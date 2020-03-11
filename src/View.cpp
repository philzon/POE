#include "View.hpp"

View::View()
{
	mStyle = Style::VIEW_VERTICAL;
	mTabSize = 4;
	mRuler = true;
	mFlags = true;
	mScrollV = 0;
	mScrollH = 0;
}

View::View(const Buffer &buffer)
{
	mStyle = Style::VIEW_VERTICAL;
	mTabSize = 4;
	mRuler = true;
	mFlags = true;
	mScrollV = 0;
	mScrollH = 0;
	mBuffer = buffer;
}

View::View(const View &other)
{
	mStyle = other.mStyle;
	mTabSize = other.mTabSize;
	mRuler = other.mRuler;
	mFlags = other.mFlags;
	mScrollV = other.mScrollV;
	mScrollH = other.mScrollH;
	mBuffer = other.mBuffer;
}

View &View::operator=(const View &other)
{
	if (this != &other)
	{
		mStyle = other.mStyle;
		mTabSize = other.mTabSize;
		mRuler = other.mRuler;
		mFlags = other.mFlags;
		mScrollV = other.mScrollV;
		mScrollH = other.mScrollH;
		mBuffer = other.mBuffer;
	}

	return *this;
}

View::~View()
{}

void View::render()
{
	render(0, 0, COLS, LINES);
}

void View::render(unsigned int left, unsigned int top, unsigned int width, unsigned int height)
{
	// Do not render anything if the buffer contains 0 lines.
	if (mBuffer.getLines() == 0)
		return;

	width = width - left;
	height = height - left;

	Cursor cursor = mBuffer.getCursor();

	// Track offsets (rulers, flags etc).
	unsigned int flagOffset = 0;
	unsigned int rulerOffset = 0;

	if (mFlags && mBuffer.hasFlags())
		flagOffset = 2;

	if (mRuler)
		rulerOffset = std::to_string(mBuffer.getLines()).size() + 1;

	// Total offset.
	unsigned int offset = flagOffset + rulerOffset + 1;

	// Adjust view if horizontal cursor goes out of view north.
	if (cursor.y < mScrollH)
		mScrollH = cursor.y;

	// Adjust view if horizontal cursor goes out of view south.
	if (cursor.y > (mScrollH + height) - 1)
		mScrollH = (cursor.y - height) + 1;

	// Adjust view if vertical cursor goes out of view west.
	if (cursor.x < mScrollV)
		mScrollV = cursor.x;

	// Adjust view if vertical cursor goes out of view east.
	if (cursor.x > (mScrollV + (width - offset)) - 1)
		mScrollV = (cursor.x - (width - offset)) + 1;

	// Set current line display.
	unsigned int line = mScrollH + 1;

	// Render all buffer in the view lines.
	for (int y = 0; y < height; ++y)
	{
		// Prevent rendering of text when out of bounds.
		if ((mScrollH + y + 1) > (mBuffer.getLines()))
			break;

		bool disabled = false;

		// Draw flag.
		if (mFlags && mBuffer.hasFlags())
		{
			switch (mBuffer.getFlag(mScrollH + y))
			{
				case Flag::FLAG_DISABLED:
					disabled = true;
					mvaddch(y + top, left, 'D');
					break;
			}
		}

		// Draw ruler.
		if (mRuler)
		{
			// Do not render rulers for lines which are disabled.
			if (!disabled)
			{
				// Allign right to left.
				int currentSize = std::to_string(line).size();
				int maxSize = std::to_string(mBuffer.getLines()).size();
				mvprintw(y + top, left + flagOffset + (maxSize - currentSize), std::to_string(line).c_str());
			}
			else
			{
				--line;
			}
		}

		for (int x = 0; x < width; ++x)
		{
			// Prevent rendering of text when out of bounds.
			if ((mScrollV + x + 1) > (mBuffer.getColumns(mScrollH + y)))
				break;

			// Print character.
			mvaddch(y + top, x + left + offset, mBuffer.get(mScrollH + y, mScrollV + x));
		}

		++line;
	}

	// Render wrap border.
	// if (mBuffer.getWrap() > 0)
	// mvaddch(cursor.y - mScrollH, mBuffer.getWrap() - mScrollV, '|');

	// Render read-only symbol on cursor.
	// if (mBuffer.isReadOnlyMode())
	// mvaddch(cursor.y - mScrollH, cursor.x - mScrollV, '*');

	// Render the cursor.
	move((cursor.y - mScrollH) + top, (cursor.x - mScrollV) + left + offset);
}

////////////////////////////////////////////////////////////////////////
// Getters
////////////////////////////////////////////////////////////////////////

Buffer *View::getBuffer()
{
	return &mBuffer;
}

////////////////////////////////////////////////////////////////////////
// Setters
////////////////////////////////////////////////////////////////////////

void View::setTabSize(unsigned int size)
{
	mTabSize = size;
}

void View::showFlags(bool state)
{
	mFlags = state;
}

void View::showRuler(bool state)
{
	mRuler = state;
}

void View::setStyle(Style style)
{
	switch (style)
	{
		case Style::VIEW_VERTICAL:
		case Style::VIEW_HORIZONTAL:
			mStyle = style;
			break;
	}
}

////////////////////////////////////////////////////////////////////////
// Private
////////////////////////////////////////////////////////////////////////
