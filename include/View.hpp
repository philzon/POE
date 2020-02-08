#ifndef VIEW_HPP
#define VIEW_HPP

#include <ncurses.h>

#include "Buffer.hpp"

enum Style
{
	VIEW_VERTICAL,
	VIEW_HORIZONTAL
};

class View
{
public:
	View();
	View(const Buffer &buffer);
	View(const View &view);
	View &operator=(const View &view);
	virtual ~View();

	void render();
	virtual void render(unsigned int left, unsigned int top, unsigned int width, unsigned int height);

	// Getters.
	Buffer *getBuffer();
	Style getStyle() const;

	// Setters.
	void setTabSize(unsigned int size);
	void showFlags(bool state);
	void showRuler(bool state);
	void setStyle(Style style);

private:
	// Rendering settings.
	unsigned int mTabSize;
	bool mRuler;
	bool mFlags;
	Style mStyle;
	unsigned int mScrollV;
	unsigned int mScrollH;

	Buffer mBuffer;
};

#endif // VIEW_HPP
