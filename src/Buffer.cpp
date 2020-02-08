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
	mHighlight.x1 = 0;
	mHighlight.y1 = 0;
	mHighlight.x2 = 0;
	mHighlight.y2 = 0;
	mPreviousColumn = 0;
	mWrap = 0;
	mTitle = "Untitled Document.txt";
}

Buffer::Buffer(const std::vector<std::string> &lines)
{
	mError = "";
	mDirty = false;
	mInsertMode = false;
	mMultilineMode = true;
	mReadOnlyMode = false;
	mCursor.x = 0;
	mCursor.y = 0;
	mHighlight.x1 = 0;
	mHighlight.y1 = 0;
	mHighlight.x2 = 0;
	mHighlight.y2 = 0;
	mPreviousColumn = 0;
	mWrap = 0;
	mTitle = "Untitled Document.txt";

	if (!lines.empty())
	{
		mLines = lines;
		mCursor.x = lines.at(lines.size() - 1).size();
		mCursor.y = lines.size() - 1;
	}
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
	mHighlight.x1 = other.mHighlight.x1;
	mHighlight.y1 = other.mHighlight.y1;
	mHighlight.x2 = other.mHighlight.x2;
	mHighlight.y2 = other.mHighlight.y2;
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
		mHighlight.x1 = other.mHighlight.x1;
		mHighlight.y1 = other.mHighlight.y1;
		mHighlight.x2 = other.mHighlight.x2;
		mHighlight.y2 = other.mHighlight.y2;
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
			if (getFlag(i) == Flag::FLAG_DISABLED)
				continue;

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
}

void Buffer::up()
{
	check();

	if (mCursor.y > 0)
		--mCursor.y;

	if (mCursor.x > mLines.at(mCursor.y).size())
		mCursor.x = mLines.at(mCursor.y).size();
}

void Buffer::down()
{
	check();

	if (mCursor.y < mLines.size() - 1)
		++mCursor.y;

	if (mCursor.x > mLines.at(mCursor.y).size())
		mCursor.x = mLines.at(mCursor.y).size();
}

void Buffer::left()
{
	check();

	if (mCursor.x > 0)
	{
		--mCursor.x;
	}
	else
	{
		if (mCursor.y > 0)
			mCursor.x = mLines.at(--mCursor.y).size();
	}

	// mPreviousColumn = mCursor.x;
}

void Buffer::right()
{
	check();

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

	// mPreviousColumn = mCursor.x;
}

void Buffer::erase()
{
	check();

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
	check();

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
	check();

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

Highlight Buffer::getHighlight() const
{
	return mHighlight;
}

int Buffer::getLines() const
{
	return mLines.size();
}

Cursor Buffer::getCursor() const
{
	return mCursor;
}

int Buffer::getFlag(unsigned int line) const
{
	if (line > mLines.size())
		return Flag::FLAG_NONE;

	std::unordered_map<unsigned int, int>::const_iterator it = mFlags.find(line);

	if (it == mFlags.end())
		return Flag::FLAG_NONE;
	
	return it->second;
}

std::string Buffer::getLine(unsigned int line) const
{
	return mLines.at(line);
}

bool Buffer::hasFlags() const
{
	return !mFlags.empty();
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

// TODO: some fucked up error about cursor limitations here!
// Probably some type casting needed or just go for integers on every-fucking-thing!
void Buffer::setCursor(unsigned int line, unsigned int column)
{
	setCursor(Cursor({line, column}));
}

void Buffer::setCursor(const Cursor &cursor)
{
	mCursor = cursor;

	if (mCursor.y > mLines.size() - 1)
		mCursor.y = mLines.size() - 1;

	if (mCursor.x > mLines.at(mCursor.y).size())
		mCursor.x = mLines.at(mCursor.y).size() - 1;
}

void Buffer::setFlag(unsigned int line, Flag flag)
{
	mFlags[line] = flag;

	// FLAG_NONE should remove the element.
	if (flag == FLAG_NONE)
		mFlags.erase(line);
}

void Buffer::setInsertMode(bool state)
{
	mInsertMode = state;
}

/*void Buffer::setHighlight(int x1, int y1, int x2, int y2)
{
	mHighlight.x1 = x1;
	mHighlight.y1 = y1;
	mHighlight.x2 = x2;
	mHighlight.y2 = y2;
}

void Buffer::setHighlight(const Highlight &highlight)
{
	mHighlight = highlight;
}*/

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

void Buffer::check()
{
	// At least one line is needed.
	if (mLines.empty())
		mLines.push_back("");
}

void Buffer::newline()
{
	// TODO: performance hit here probably!
	// Bump all flags.
	std::unordered_map<unsigned int, int> flags;

	for (std::pair<unsigned int, int> element : mFlags)
	{
		if (element.first >= mCursor.y)
			flags[element.first + 1] = element.second;
		else
			flags[element.first] = element.second;
	}

	mFlags = flags;

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
	if (mCursor.y > 0)
	{
		// Disable any flags on this line.
		setFlag(mCursor.y, FLAG_NONE);

		// Drag down flags.
		// TODO: performance hit here probably!
		// Bump all flags.
		std::unordered_map<unsigned int, int> flags;

		for (std::pair<unsigned int, int> element : mFlags)
		{
			if (element.first >= mCursor.y)
				flags[element.first - 1] = element.second;
			else
				flags[element.first] = element.second;
		}

		mFlags = flags;

		// Get the rest of the current line's content if there are any.
		std::string buf = mLines.at(mCursor.y);

		// Remove current line and reset cursor.
		mLines.erase(mLines.begin() + mCursor.y);

		// Reset cursor to new line.
		mCursor.x = mLines.at(--mCursor.y).size();

		// Append the previous content to the current line.
		mLines.at(mCursor.y) += buf;
	}
}
