#include "Editor.hpp"

Editor::Editor()
{
	mIsRunning = true;
	mShowRuler = true;
}

Editor::~Editor()
{}

void Editor::input(int ch)
{
	switch (ch)
	{
	// Disable line (CTRL + D).
	case ('d' & 037):
		if (mView.getBuffer()->getFlag(mView.getBuffer()->getCursor().y) == Flag::FLAG_DISABLED)
			mView.getBuffer()->setFlag(mView.getBuffer()->getCursor().y, Flag::FLAG_NONE);
		else
			mView.getBuffer()->setFlag(mView.getBuffer()->getCursor().y, Flag::FLAG_DISABLED);
		break;
	// Close buffer (CTRL + Q).
	case ('q' & 037):
		close();
		break;
	// Save (CTRL + S).
	case ('s' & 037):
		save(mView.getBuffer()->getTitle());
		break;
	case KEY_UP:
		mView.getBuffer()->up();
		break;
	case KEY_DOWN:
		mView.getBuffer()->down();
		break;
	case KEY_LEFT:
		mView.getBuffer()->left();
		break;
	case KEY_RIGHT:
		mView.getBuffer()->right();
		break;
	case KEY_IC:
		mView.getBuffer()->setInsertMode(!mView.getBuffer()->isInsertMode());
		break;
	case KEY_BACKSPACE:
	case '\b':
	case 127:
		mView.getBuffer()->erase();
		break;
	case KEY_DC:
		mView.getBuffer()->del();
		break;
	// Do nothing with the resize signal.
	case KEY_RESIZE:break;
	default:
		mView.getBuffer()->insert(static_cast<char>(ch));
		break;
	}
}

void Editor::render()
{
	mView.render(0, 0, COLS, LINES);
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

	if (!mView.getBuffer()->read(file, append))
	{
		mError = mView.getBuffer()->getError();
		return false;
	}

	mView.getBuffer()->setTitle(file);

	// TODO: cursor can be set to an invalid position.
	// Set cursor.
	Cursor cursor = mView.getBuffer()->getCursor();

	if (!append)
		cursor = Cursor({0, 0});

	if (tokens.size() > 1)
		cursor.y = std::strtol(tokens.at(1).c_str(), nullptr, 10) - 1;

	if (tokens.size() > 2)
		cursor.x = std::strtol(tokens.at(2).c_str(), nullptr, 10) - 1;

	mView.getBuffer()->setCursor(cursor);

	// Apply buffer modes.
	if (readOnlyMode)
		mView.getBuffer()->setReadOnlyMode(readOnlyMode);

	if (insertMode)
		mView.getBuffer()->setReadOnlyMode(insertMode);

	return true;
}

bool Editor::save(const std::string &path)
{
	if (!mView.getBuffer()->write(path))
	{
		mError = mView.getBuffer()->getError();
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
