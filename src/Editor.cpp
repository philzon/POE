#include "Editor.hpp"

Editor::Editor()
{
	mIsRunning = true;
	mShowRuler = true;
	mScrollV = 0;
	mScrollH = 0;
}

Editor::~Editor()
{}

void Editor::input(int ch)
{
	switch (ch)
	{
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
	// Screen position and size.
	const int left = 0;
	const int top = 0;
	const int width = COLS - left;
	const int height = LINES - top;

	Cursor cursor = mBuffer.getCursor();

	// Track offsets (rulers, flags etc).
	unsigned int leftOffset = 0;
	unsigned int topOffset = 0;

	renderTitle(left, top, width, height, leftOffset, topOffset);

	// Adjust view if horizontal cursor goes out of view north.
	if (cursor.y < mScrollH)
		mScrollH = cursor.y;

	// Adjust view if horizontal cursor goes out of view south.
	if (cursor.y > (mScrollH + (height - topOffset)) - 1)
		mScrollH = (cursor.y - (height - topOffset)) + 1;

	if (mShowRuler)
		renderLineNumbers(left, top, width, height, leftOffset, topOffset);

	// Adjust view if vertical cursor goes out of view west.
	if (cursor.x < mScrollV)
		mScrollV = cursor.x;

	// Adjust view if vertical cursor goes out of view east.
	if (cursor.x > (mScrollV + (width - leftOffset)) - 1)
		mScrollV = (cursor.x - (width - leftOffset)) + 1;

	// Render all buffer in the view lines.
	renderText(left, top, width, height, leftOffset, topOffset);

	// Render wrap boundries.
	if (mBuffer.getWrap() > 0)
		renderWrapper(left, top, width, height, leftOffset, topOffset);

	// Render the cursor.
	move((cursor.y - mScrollH) + top + topOffset, (cursor.x - mScrollV) + left + leftOffset);
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

	while (std::getline(stream, token, ':'))
		tokens.push_back(token);

	bool append = false;
	bool readOnlyMode = false;

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

	// Set cursor.
	Cursor cursor = mBuffer.getCursor();

	if (!append)
	{
		cursor = Cursor({0, 0});
		mBuffer.setDirty(false);
	}

	if (tokens.size() > 1 && std::all_of(tokens.at(1).begin(), tokens.at(1).end(), ::isdigit))
		cursor.y = std::strtol(tokens.at(1).c_str(), nullptr, 10) - 1;

	if (tokens.size() > 2 && std::all_of(tokens.at(2).begin(), tokens.at(2).end(), ::isdigit))
		cursor.x = std::strtol(tokens.at(2).c_str(), nullptr, 10) - 1;

	mBuffer.setCursor(cursor);

	// Apply buffer modes.
	if (readOnlyMode)
		mBuffer.setReadOnlyMode(readOnlyMode);

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

void Editor::renderTitle(int left, int top, int width, int height, unsigned int &leftOffset, unsigned int &topOffset)
{
	std::string title = "-- ";
	title = title + mBuffer.getTitle();

	if (mBuffer.isDirty())
		title = title + "*";

	if (mBuffer.isReadOnlyMode())
		title = title + " " + "[R]";

	title = title + " " + std::string(COLS - title.size() - 1, '-');

	mvprintw(0, 0, title.c_str());

	// Indicate that other rendering operations will have to
	// be shifted down.
	++topOffset;
}

void Editor::renderLineNumbers(int left, int top, int width, int height, unsigned int &leftOffset, unsigned int &topOffset)
{
	unsigned int line = mScrollH;

	for (int y = 0; y < height; ++y)
	{
		++line;

		if (y > mBuffer.getLines() - 1)
			break;

		// Allign right to left.
		int currentSize = std::to_string(line).size();
		int maxSize = std::to_string(mBuffer.getLines()).size();

		mvprintw(y + top + topOffset, left + leftOffset + (maxSize - currentSize), std::to_string(line).c_str());
	}

	// Indicate that other rendering operations will have to
	// be shifted to the right.
	leftOffset += std::to_string(mBuffer.getLines()).size() + 1;
}

void Editor::renderText(int left, int top, int width, int height, unsigned int &leftOffset, unsigned int &topOffset)
{
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
			mvaddch(y + top + topOffset, x + left + leftOffset, mBuffer.get(mScrollH + y, mScrollV + x));
		}
	}
}

void Editor::renderWrapper(int left, int top, int width, int height, unsigned int &leftOffset, unsigned int &topOffset)
{
	mvaddch(mBuffer.getCursor().y - mScrollH + topOffset, mBuffer.getWrap() - mScrollV + leftOffset, '<');
}
