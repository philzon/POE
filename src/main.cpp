#include <string>
#include <vector>

#include "Application.hpp"

int main(int argc, char **argv)
{
	std::vector<std::string> commands;

	for (int i = 0; i < argc; ++i)
		commands.push_back(std::string(argv[i]));

	return Application(commands).run();
}
