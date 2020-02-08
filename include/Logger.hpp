#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <memory>

#include "Buffer.hpp"

class Logger
{
public:
	static std::shared_ptr<Logger> getInstance()
	{
		std::shared_ptr<Logger> instance(new Logger());
		return instance;
	}

	void debug(const std::string &message);
	void info(const std::string &message);
	void warning(const std::string &message);
	void error(const std::string &message);
	void critical(const std::string &message);

	// Getters.
	Buffer *getBuffer();

	// Setters.
	void setBuffer(Buffer *buffer);

private:
	Logger();
	Logger(const Logger &other) = delete;
	void operator=(const Logger &other) = delete;

	Buffer *mBuffer;
};

#endif // LOGGER_HPP
