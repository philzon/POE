#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <ncurses.h>

#include "Buffer.hpp"

class Editor
{
public:
	Editor();
	virtual ~Editor();

	virtual void input(int ch);
	virtual void render();

	void close();
	bool open(const std::string &path);
	bool save(const std::string &path);

	// Getters.
	std::string getError() const;
	bool isRunning() const;

private:
	// Helper functions for rendering.
	void renderTitle(unsigned int left, unsigned int top, unsigned int width, unsigned int height, unsigned int &leftOffset, unsigned int &topOffset);
	void renderLineNumbers(unsigned int left, unsigned int top, unsigned int width, unsigned int height, unsigned int &leftOffset, unsigned int &topOffset);
	void renderText(unsigned int left, unsigned int top, unsigned int width, unsigned int height, unsigned int &leftOffset, unsigned int &topOffset);
	void renderWrapper(unsigned int left, unsigned int top, unsigned int width, unsigned int height, unsigned int &leftOffset, unsigned int &topOffset);

	bool mIsRunning;

	unsigned int mScrollV;
	unsigned int mScrollH;
	bool mShowRuler;

	std::string mError;

	Buffer mBuffer;
};

#endif // EDITOR_HPP
