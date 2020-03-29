#include "Buffer.hpp"

Buffer::Buffer()
{
	mError = "";
	mDirty = false;
	mInsertMode = false;
	mMultilineMode = true;
	mReadOnlyMode = false;
	mCursor.x = 0;
	mCursor.y = 0;
	mPreviousColumn = 0;
	mWrap = 0;
	mTitle = "Untitled Document.txt";
	mLines.push_back(std::string(""));
}

Buffer::Buffer(const std::vector<std::string> &lines)
{
	mError = "";
	mDirty = false;
	mInsertMode = false;
	mMultilineMode = true;
	mReadOnlyMode = false;
	mPreviousColumn = 0;
	mWrap = 0;
	mTitle = "Untitled Document.txt";

	mLines.push_back(std::string(""));

	if (!lines.empty())
		mLines = lines;

	mCursor.x = lines.at(lines.size() - 1).size();
	mCursor.y = lines.size() - 1;
}

Buffer::Buffer(const Buffer &other)
{
	mError = other.mError;
	mDirty = other.mDirty;
	mInsertMode = other.mInsertMode;
	mMultilineMode = other.mMultilineMode;
	mReadOnlyMode = other.mReadOnlyMode;
	mCursor.x = other.mCursor.x;
	mCursor.y = other.mCursor.y;
	mPreviousColumn = other.mPreviousColumn;
	mWrap = other.mWrap;
	mTitle = other.mTitle;
	mLines = other.mLines;
}

Buffer &Buffer::operator=(const Buffer &other)
{
	if (this != &other)
	{
		mError = other.mError;
		mDirty = other.mDirty;
		mInsertMode = other.mInsertMode;
		mMultilineMode = other.mMultilineMode;
		mReadOnlyMode = other.mReadOnlyMode;
		mCursor.x = other.mCursor.x;
		mCursor.y = other.mCursor.y;
		mPreviousColumn = other.mPreviousColumn;
		mWrap = other.mWrap;
		mTitle = other.mTitle;
		mLines = other.mLines;
	}

	return *this;
}

Buffer::~Buffer()
{}

bool Buffer::read(const std::string &path)
{
	return read(path, false);
}

bool Buffer::read(const std::string &path, bool append)
{
	try
	{
		std::ifstream stream;
		stream.open(path);

		if (!stream.is_open())
		{
			mError = "Could not open " + path;
			return false;
		}

		stream.seekg(0, std::ios::end);
		std::size_t size = stream.tellg();
		stream.seekg(0);
		std::string data(size, ' ');
		stream.read(&data[0], size);

		if (!append)
			clear();

		insert(data);
	}
	catch(const std::exception& e)
	{
		mError = e.what();
		return false;
	}

	return true;
}

bool Buffer::write(const std::string &path)
{
	try
	{
		std::ofstream stream(path);

		for (std::size_t i = 0; i < mLines.size(); ++i)
		{
			stream << mLines.at(i);

			if (i < mLines.size() - 1)
				stream << "\n";
		}
	}
	catch(const std::exception &e)
	{
		mError = e.what();
		return false;
	}

	mDirty = false;

	return true;
}

void Buffer::clear()
{
	if (!mLines.empty())
		mDirty = true;

	mLines.clear();

	mPreviousColumn = 0;
	mCursor.x = 0;
	mCursor.y = 0;

	mLines.push_back(std::string(""));
}

void Buffer::up()
{
	if (mCursor.y > 0)
		--mCursor.y;

	// Retain horizontal position from previous recorded position.
	mCursor.x = mPreviousColumn;

	// Boundry check - keep cursor away from EOL.
	if (mCursor.x > mLines.at(mCursor.y).size())
		mCursor.x = mLines.at(mCursor.y).size();
}

void Buffer::down()
{
	if (mCursor.y < mLines.size() - 1)
		++mCursor.y;

	// Retain horizontal position from previous recorded position.
	mCursor.x = mPreviousColumn;

	// Boundry check - keep cursor away from invalid position.
	if (mCursor.x > mLines.at(mCursor.y).size())
		mCursor.x = mLines.at(mCursor.y).size();
}

void Buffer::left()
{
	if (mCursor.x > 0)
	{
		--mCursor.x;
	}
	else
	{
		if (mCursor.y > 0)
			mCursor.x = mLines.at(--mCursor.y).size();
	}

	// Update last known horizontal position.
	mPreviousColumn = mCursor.x;
}

void Buffer::right()
{
	if (mCursor.x < mLines.at(mCursor.y).size())
	{
		++mCursor.x;
	}
	else
	{
		if (mCursor.y < mLines.size() - 1)
		{
			++mCursor.y;
			mCursor.x = 0;
		}
	}

	// Update last known horizontal position.
	mPreviousColumn = mCursor.x;
}

void Buffer::erase()
{
	if (mReadOnlyMode)
		return;

	if (mCursor.x == 0)
		removeline();
	else
		mLines.at(mCursor.y).erase(mCursor.x-- - 1, 1);

	mDirty = true;
}

void Buffer::del()
{
	if (mReadOnlyMode)
		return;

	if (mCursor.x == mLines.at(mCursor.y).size())
	{
		if (mCursor.y < mLines.size() - 1)
		{
			mLines.at(mCursor.y) += mLines.at(mCursor.y + 1);
			mLines.erase(mLines.begin() + mCursor.y + 1);
			mDirty = true;
		}
	}
	else
	{
		if (!mLines.at(mCursor.y).empty())
		{
			mLines.at(mCursor.y).erase(mCursor.x, 1);
			mDirty = true;
		}
	}
}

void Buffer::insert(char ch)
{
	if (mReadOnlyMode)
		return;

	if (isMultilineMode() && (mWrap > 0) && (mCursor.x == mWrap) && (ch != '\n' && ch != '\r'))
		newline();

	if (isMultilineMode() && (ch == '\n' || ch == '\r'))
		newline();
	else if (isInsertMode() && (mCursor.x < getColumns(mCursor.y)))
		mLines.at(mCursor.y).at(mCursor.x++) = ch;
	else
		mLines.at(mCursor.y).insert(mCursor.x++, std::string(1, ch));

	mDirty = true;

	// Update last known horizontal position.
	mPreviousColumn = mCursor.x;
}

void Buffer::insert(const std::string &text)
{
	if (mReadOnlyMode)
		return;

	for (char c : text)
		insert(c);
}

////////////////////////////////////////////////////////////////////////
// Getters
////////////////////////////////////////////////////////////////////////

bool Buffer::isDirty() const
{
	return mDirty;
}

bool Buffer::isInsertMode() const
{
	return mInsertMode;
}

bool Buffer::isMultilineMode() const
{
	return mMultilineMode;
}

bool Buffer::isReadOnlyMode() const
{
	return mReadOnlyMode;
}

char Buffer::get(unsigned int line, unsigned int column) const
{
	return mLines.at(line).at(column);
}

int Buffer::getColumns(unsigned int line) const
{
	return mLines.at(line).size();
}

std::string Buffer::getError() const
{
	return mError;
}

int Buffer::getLines() const
{
	return mLines.size();
}

Cursor Buffer::getCursor() const
{
	return mCursor;
}

std::string Buffer::getLine(unsigned int line) const
{
	return mLines.at(line);
}

std::string Buffer::toString() const
{
	std::string str;

	for (std::size_t i = 0; i < mLines.size(); ++i)
	{
		str.append(getLine(i));

		if (i != mLines.size() - 1)
			str.append("\n");
	}

	return str;
}

std::string Buffer::getTitle() const
{
	return mTitle;
}

unsigned int Buffer::getWrap() const
{
	return mWrap;
}

////////////////////////////////////////////////////////////////////////
// Setters
////////////////////////////////////////////////////////////////////////

void Buffer::setCursor(unsigned int line, unsigned int column)
{
	setCursor(Cursor({column, line}));
}

void Buffer::setCursor(const Cursor &cursor)
{
	mCursor = cursor;

	if (mCursor.y > mLines.size() - 1)
		mCursor.y = mLines.size() - 1;

	if (mCursor.x > mLines.at(mCursor.y).size())
		mCursor.x = mLines.at(mCursor.y).size();
}

void Buffer::setDirty(bool state)
{
	mDirty = state;
}

void Buffer::setInsertMode(bool state)
{
	mInsertMode = state;
}

void Buffer::setMultilineMode(bool state)
{
	mMultilineMode = state;
}

void Buffer::setReadOnlyMode(bool state)
{
	mReadOnlyMode = state;
}

void Buffer::setTitle(const std::string &title)
{
	mTitle = title;
}

void Buffer::setWrap(unsigned int amount)
{
	mWrap = amount;
}

////////////////////////////////////////////////////////////////////////
// Private
////////////////////////////////////////////////////////////////////////

void Buffer::newline()
{
	// Get the rest of the current line's content if there are any.
	std::string buf = mLines.at(mCursor.y).substr(mCursor.x, mLines.at(mCursor.y).size());

	// Remove the copied content, if there ever was.
	if (buf.size())
		mLines.at(mCursor.y).erase(mCursor.x, mLines.at(mCursor.y).size());

	// Insert a new line with the cut buffer.
	mLines.insert(mLines.begin() + ++mCursor.y, buf);

	// Reset line for x.
	mCursor.x = 0;
}

void Buffer::removeline()
{
	// Get the rest of the current line's content if there are any.
	std::string buf = mLines.at(mCursor.y);

	// Remove current line and reset cursor.
	mLines.erase(mLines.begin() + mCursor.y);

	// Reset cursor to new line.
	mCursor.x = mLines.at(--mCursor.y).size();

	// Append the previous content to the current line.
	mLines.at(mCursor.y) += buf;
}
