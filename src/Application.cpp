#include "Application.hpp"

Application::Application(const std::vector<std::string> &flags)
{
	mHelp = false;
	mVersion = false;

	parse(flags);

	if (mHelp)
	{
		usage();
		mEditor.close();
	}

	if (mVersion)
	{
		version();
		mEditor.close();
	}

	if (mFiles.empty())
	{
		Tab tab;
		tab.add(View());
		mEditor.add(tab);
	}
	else
	{
		for (std::string file : mFiles)
		{
			Tab tab;
			tab.add(View());
			mEditor.add(tab);

			// Do not handle the error! If a file could not
			// be loaded then we just leave the buffer empty.
			mEditor.open(file);
		}
	}

	if (mEditor.isRunning())
	{
		initscr();
		keypad(stdscr, true);
		raw();
		noecho();
	}
}

Application::~Application()
{
	endwin();
}

int Application::run()
{
	// Exit if failures occured in the constructor.
	// TODO: exit based on error 0-1.
	if (!mEditor.isRunning())
		return 0;

	while (mEditor.isRunning())
	{
		// Clear everything on screen.
		erase();

		// Draw editor's content.
		mEditor.render();

		// Update changes to screen.
		refresh();

		// Get keyboard input.
		mEditor.input(getch());
	}

	return 0;
}

void Application::parse(const std::vector<std::string> &flags)
{
	for (std::size_t i = 0; i < flags.size(); ++i)
	{
		std::string flag = flags.at(i);
		std::string argument = "";

		if (i + 1 < flags.size())
			argument = flags.at(i + 1);

		if (flag == "-h" || flag == "--help")
		{
			mHelp = true;
			continue;
		}

		if (flag == "-v" || flag == "--version")
		{
			mVersion = true;
			continue;
		}

		// Skip the path to executable.
		if (i > 0)
			mFiles.push_back(flag);
	}
}

void Application::usage()
{
	std::cout << "Usage:  " << BUILD_TITLE << " [FLAGS] [FILES:LINE:COL:MODE...]\n";
	std::cout << "\n";
	std::cout << "Philzon's Own Editor\n";
	std::cout << "\n";
	std::cout << "Modes:\n";
	std::cout << "  a   append data to current buffer\n";
	std::cout << "  i   enable insert mode\n";
	std::cout << "  r   enable read-only mode\n";
	std::cout << "\n";
	std::cout << "Options:\n";
	std::cout << "  -h, --help      print this and exit\n";
	std::cout << "  -v, --version   print version and exit\n";
}

void Application::version()
{
	std::cout << "Version: " << BUILD_VERSION << "\n";
	std::cout << "Commit:  " << BUILD_COMMIT << "\n";
	std::cout << "Date:    " << BUILD_DATE << "\n";
}
