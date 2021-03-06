#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include <ncurses.h>

#include "Editor.hpp"

#ifndef BUILD_TITLE
#define BUILD_TITLE "app"
#endif
#ifndef BUILD_VERSION
#define BUILD_VERSION "0.0.0"
#endif
#ifndef BUILD_COMMIT
#define BUILD_COMMIT "0000000"
#endif
#ifndef BUILD_DATE
#define BUILD_DATE "1970-01-01 00:00:00"
#endif

class Application
{
public:
	Application(const std::vector<std::string> &flags);
	virtual ~Application();

	virtual int run();

private:
	void parse(const std::vector<std::string> &flags);
	void usage();
	void version();

	std::vector<std::string> mFiles;

	Editor mEditor;

	bool mHelp;
	bool mVersion;
};

#endif // APPLICATION_HPP
