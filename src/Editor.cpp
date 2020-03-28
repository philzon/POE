#include "Editor.hpp"

Editor::Editor()
{
	mIsRunning = true;
	mShowRuler = true;
	mShowFlags = true;
	mScrollV = 0;
	mScrollH = 0;
}

Editor::~Editor()
{}

void Editor::input(int ch)
{
	switch (ch)
	{
	// Disable line (CTRL + D).
	case ('d' & 037):
		if (mBuffer.getFlag(mBuffer.getCursor().y) == Flag::FLAG_DISABLED)
			mBuffer.setFlag(mBuffer.getCursor().y, Flag::FLAG_NONE);
		else
			mBuffer.setFlag(mBuffer.getCursor().y, Flag::FLAG_DISABLED);
		break;
	// Close buffer (CTRL + Q).
	case ('q' & 037):
		close();
		break;
	// Save (CTRL + S).
	case ('s' & 037):
		save(mBuffer.getTitle());
		break;
	case KEY_UP:
		mBuffer.up();
		break;
	case KEY_DOWN:
		mBuffer.down();
		break;
	case KEY_LEFT:
		mBuffer.left();
		break;
	case KEY_RIGHT:
		mBuffer.right();
		break;
	case KEY_IC:
		mBuffer.setInsertMode(!mBuffer.isInsertMode());
		break;
	case KEY_BACKSPACE:
	case '\b':
	case 127:
		mBuffer.erase();
		break;
	case KEY_DC:
		mBuffer.del();
		break;
	// Do nothing with the resize signal.
	case KEY_RESIZE:break;
	default:
		mBuffer.insert(static_cast<char>(ch));
		break;
	}
}

void Editor::render()
{
	// Do not render anything if the buffer contains 0 lines.
	if (mBuffer.getLines() == 0)
		return;

	// Screen position and size.
	int left = 0;
	int top = 0;
	int width = COLS;
	int height = LINES;

	width = width - left;
	height = height - left;

	Cursor cursor = mBuffer.getCursor();

	// Track offsets (rulers, flags etc).
	unsigned int leftOffset = 0;
	unsigned int topOffset = 0;

	// Adjust view if horizontal cursor goes out of view north.
	if (cursor.y < mScrollH)
		mScrollH = cursor.y;

	// Adjust view if horizontal cursor goes out of view south.
	if (cursor.y > (mScrollH + (height - topOffset)) - 1)
		mScrollH = (cursor.y - (height - topOffset)) + 1;

	if (mShowFlags && mBuffer.hasFlags())
		renderFlags(left, top, width, height, leftOffset, topOffset);

	if (mShowRuler)
		renderLinesNumbers(left, top, width, height, leftOffset, topOffset);

	// Adjust view if vertical cursor goes out of view west.
	if (cursor.x < mScrollV)
		mScrollV = cursor.x;

	// Adjust view if vertical cursor goes out of view east.
	if (cursor.x > (mScrollV + (width - leftOffset)) - 1)
		mScrollV = (cursor.x - (width - leftOffset)) + 1;

	// Render all buffer in the view lines.
	for (int y = 0; y < height; ++y)
	{
		// Prevent rendering of text when out of bounds.
		if ((mScrollH + y + 1) > (mBuffer.getLines()))
			break;

		for (int x = 0; x < width; ++x)
		{
			// Prevent rendering of text when out of bounds.
			if ((mScrollV + x + 1) > (mBuffer.getColumns(mScrollH + y)))
				break;

			// Print character.
			mvaddch(y + top, x + left + leftOffset, mBuffer.get(mScrollH + y, mScrollV + x));
		}
	}

	// Render wrap boundries.
	if (mBuffer.getWrap() > 0)
		renderWrapper(left, top, width, height, leftOffset, topOffset);

	// Render the cursor.
	move((cursor.y - mScrollH) + top, (cursor.x - mScrollV) + left + leftOffset);
}

void Editor::close()
{
	mIsRunning = false;
}

bool Editor::open(const std::string &path)
{
	// Split string into the following tokens:
	// FILE    (string)
	// LINE    (int)
	// COLUMN  (int)
	// MODES   (string)
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream stream(path);

	// TODO: Token parsing is broken - fix it, motherfucker!
	while (std::getline(stream, token, ':'))
		tokens.push_back(token);

	bool append = false;
	bool readOnlyMode = false;
	bool insertMode = false;

	// Parse buffer modes.
	if (tokens.size() > 3)
	{
		for (char c : tokens.at(3))
		{
			switch (c)
			{
				case 'a':
					append = true;
					break;
				case 'i':
					insertMode = true;
					break;
				case 'r':
					readOnlyMode = true;
					break;
			}
		}
	}

	// Load data from file.
	std::string file;

	if (tokens.size() > 0)
		file = tokens.at(0);

	if (!mBuffer.read(file, append))
	{
		mError = mBuffer.getError();
		return false;
	}

	mBuffer.setTitle(file);

	// TODO: cursor can be set to an invalid position.
	// Set cursor.
	Cursor cursor = mBuffer.getCursor();

	if (!append)
		cursor = Cursor({0, 0});

	if (tokens.size() > 1)
		cursor.y = std::strtol(tokens.at(1).c_str(), nullptr, 10) - 1;

	if (tokens.size() > 2)
		cursor.x = std::strtol(tokens.at(2).c_str(), nullptr, 10) - 1;

	mBuffer.setCursor(cursor);

	// Apply buffer modes.
	if (readOnlyMode)
		mBuffer.setReadOnlyMode(readOnlyMode);

	if (insertMode)
		mBuffer.setReadOnlyMode(insertMode);

	mBuffer.setWrap(10);

	return true;
}

bool Editor::save(const std::string &path)
{
	if (!mBuffer.write(path))
	{
		mError = mBuffer.getError();
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////
// Getters
////////////////////////////////////////////////////////////////////////

std::string Editor::getError() const
{
	return mError;
}

bool Editor::isRunning() const
{
	return mIsRunning;
}

////////////////////////////////////////////////////////////////////////
// Setters
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Private
////////////////////////////////////////////////////////////////////////

void Editor::renderFlags(int left, int top, int width, int height, unsigned int &leftOffset, unsigned int &topOffset)
{
	for (int y = 0; y < height; ++y)
	{
		bool disabled = false;

		// Draw flag.
		if (mShowFlags && mBuffer.hasFlags())
		{
			switch (mBuffer.getFlag(mScrollH + y))
			{
				case Flag::FLAG_DISABLED:
					disabled = true;
					mvaddch(y + top + topOffset, left + leftOffset, 'D');
					break;
			}
		}
	}

	// Indicate that other rendering operations will have to
	// be shifted to the right.
	leftOffset += 2;
}

void Editor::renderLinesNumbers(int left, int top, int width, int height, unsigned int &leftOffset, unsigned int &topOffset)
{
	unsigned int line = mScrollH;

	for (int y = 0; y < height; ++y)
	{
		bool disabled = false;

		// Check if line is disabled from flags.
		if (mBuffer.hasFlags())
		{
			switch (mBuffer.getFlag(mScrollH + y))
			{
				case Flag::FLAG_DISABLED:
					disabled = true;
					break;
			}
		}

		// Do not render rulers for lines which are disabled.
		if (!disabled)
		{
			++line;

			// Allign right to left.
			int currentSize = std::to_string(line).size();
			int maxSize = std::to_string(mBuffer.getLines()).size();

			mvprintw(y + top + topOffset, left + leftOffset + (maxSize - currentSize), std::to_string(line).c_str());
		}
	}

	// Indicate that other rendering operations will have to
	// be shifted to the right.
	leftOffset += std::to_string(mBuffer.getLines()).size() + 1;
}

void Editor::renderWrapper(int left, int top, int width, int height, unsigned int &leftOffset, unsigned int &topOffset)
{
	mvaddch(mBuffer.getCursor().y - mScrollH + topOffset, mBuffer.getWrap() - mScrollV + leftOffset, '<');
}
