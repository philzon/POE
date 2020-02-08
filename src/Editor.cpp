#include "Editor.hpp"

Editor::Editor()
{
	mIsRunning = true;
	mCurrentTab = 0;
	mShowRuler = true;
}

Editor::~Editor()
{}

void Editor::input(int ch)
{
	Buffer *buffer = getBuffer();

	if (!buffer)
	{
		mLogger->warning("Could not write to invalid buffer - action ignored");
		return;
	}

	switch (ch)
	{
	// Copy (CTRL + C).
	case ('c' & 037):
		copy();
		break;
	// Disable line (CTRL + D).
	case ('d' & 037):
		if (buffer->getFlag(buffer->getCursor().y) == Flag::FLAG_DISABLED)
			buffer->setFlag(buffer->getCursor().y, Flag::FLAG_NONE);
		else
			buffer->setFlag(buffer->getCursor().y, Flag::FLAG_DISABLED);
		break;
	// Open code execution (CTRL + E).
	case ('e' & 037):
		// TODO: implement me!
		break;
	// Move to next view.
	case ('n' & 037):
		next();
		break;
	// Move to previous view.
	case ('p' & 037):
		previous();
		break;
	// Open (CTRL + O).
	case ('o' & 037):
		// TODO: implement me!
		break;
	// Close buffer (CTRL + Q).
	case ('q' & 037):
		remove();
		break;
	// Toggle ruler (CTRL + R)
	case ('r' & 037):
		// TODO:
		break;
	// Save (CTRL + S).
	case ('s' & 037):
		save(buffer->getTitle());
		break;
	// Paste (CTRL + V).
	case ('v' & 037):
		paste();
		break;
	// Close? (CTRL + W).
	case ('w' & 037):
		// TODO: implement me!
		break;
	// Cut (CTRL + X).
	case ('x' & 037):
		cut();
		break;
	case KEY_UP:
		buffer->up();
		break;
	case KEY_DOWN:
		buffer->down();
		break;
	case KEY_LEFT:
		buffer->left();
		break;
	case KEY_RIGHT:
		buffer->right();
		break;
	case KEY_IC:
		buffer->setInsertMode(!buffer->isInsertMode());
		break;
	case KEY_BACKSPACE:
	case '\b':
	case 127:
		buffer->erase();
		break;
	case KEY_DC:
		buffer->del();
		break;
	// Do nothing with the resize signal.
	case KEY_RESIZE:break;
	default:
		buffer->insert(static_cast<char>(ch));
		break;
	}
}

void Editor::render()
{
	if (mTabs.empty())
		return;

	mTabs.at(mCurrentTab).render(COLS, LINES);
}

void Editor::add(const Tab &tab)
{
	mTabs.push_back(tab);
	mCurrentTab = mTabs.size() - 1;
}

void Editor::close()
{
	mIsRunning = false;
}

bool Editor::open(const std::string &path)
{
	Buffer *buffer = getBuffer();

	if (!buffer)
	{
		mLogger->error("Could not open file to an invalid buffer");
		return false;
	}

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

	if (!buffer->read(file, append))
	{
		mError = buffer->getError();
		return false;
	}

	buffer->setTitle(file);

	// TODO: cursor can be set to an invalid position.
	// Set cursor.
	Cursor cursor = buffer->getCursor();

	if (!append)
		cursor = Cursor({0, 0});

	if (tokens.size() > 1)
		cursor.y = std::strtol(tokens.at(1).c_str(), nullptr, 10) - 1;

	if (tokens.size() > 2)
		cursor.x = std::strtol(tokens.at(2).c_str(), nullptr, 10) - 1;

	buffer->setCursor(cursor);

	// Apply buffer modes.
	if (readOnlyMode)
		buffer->setReadOnlyMode(readOnlyMode);

	if (insertMode)
		buffer->setReadOnlyMode(insertMode);

	return true;
}

bool Editor::save(const std::string &path)
{
	Buffer *buffer = getBuffer();

	if (!buffer)
	{
		mLogger->error("Could not save file from an invalid buffer");
		return false;
	}

	if (!buffer->write(path))
	{
		mError = buffer->getError();
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
	return mIsRunning && !mTabs.empty();
}

////////////////////////////////////////////////////////////////////////
// Setters
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Private
////////////////////////////////////////////////////////////////////////

void Editor::cut()
{
	// TODO:
}

void Editor::copy()
{
	// TODO:
}

void Editor::paste()
{
	// TODO:
}

void Editor::next()
{
	if (mCurrentTab < mTabs.size() - 1)
		++mCurrentTab;
	else
		mCurrentTab = 0;
}

void Editor::previous()
{
	if (mCurrentTab > 0)
		--mCurrentTab;
	else
		mCurrentTab = mTabs.size() - 1;
}

void Editor::remove()
{
	if (mTabs.empty())
		return;

	// Remove selected view from tab.
	mTabs.at(mCurrentTab).remove();

	// If all views are removed, remove the tab.
	if (mTabs.at(mCurrentTab).size() == 0)
		mTabs.erase(mTabs.begin() + mCurrentTab);

	// Reset tab selection if out of bounds.
	if (mCurrentTab > mTabs.size() - 1)
		mCurrentTab = mTabs.size() - 1;
}

Buffer *Editor::getBuffer()
{
	if (mTabs.empty())
		return nullptr;

	return mTabs.at(mCurrentTab).getBuffer();
}
