#ifndef EDITOR_HPP
#define EDITOR_HPP

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
	void renderTitle(int left, int top, int width, int height, unsigned int &leftOffset, unsigned int &topOffset);
	void renderLineNumbers(int left, int top, int width, int height, unsigned int &leftOffset, unsigned int &topOffset);
	void renderText(int left, int top, int width, int height, unsigned int &leftOffset, unsigned int &topOffset);
	void renderWrapper(int left, int top, int width, int height, unsigned int &leftOffset, unsigned int &topOffset);

	bool mIsRunning;

	int mScrollV;
	int mScrollH;
	bool mShowRuler;

	std::string mError;

	Buffer mBuffer;
};

#endif // EDITOR_HPP
