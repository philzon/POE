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
#include "Logger.hpp"
#include "Tab.hpp"
#include "View.hpp"

class Editor
{
public:
	Editor();
	virtual ~Editor();

	virtual void input(int ch);
	virtual void render();

	void add(const Tab &tab);
	void close();
	bool open(const std::string &path);
	bool save(const std::string &path);

	// Getters.
	std::string getError() const;
	bool isRunning() const;

	void showRuler(bool state);

private:
	void cut();
	void copy();
	void paste();
	void next();
	void previous();
	void remove();
	Buffer *getBuffer();

	bool mIsRunning;
	bool mShowRuler;
	bool mShowFlags;

	std::string mError;
	std::string mClipboard;
	std::string mCommand;

	std::vector<Tab> mTabs;
	unsigned int mCurrentTab;

	std::shared_ptr<Logger> mLogger;
};

#endif // EDITOR_HPP
