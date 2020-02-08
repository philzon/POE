#include "Logger.hpp"

void Logger::debug(const std::string &message)
{
	mBuffer->insert("DEB  " + message + "\n");
}

void Logger::info(const std::string &message)
{
	mBuffer->insert("INF  " + message + "\n");
}

void Logger::warning(const std::string &message)
{
	mBuffer->insert("WAR  " + message + "\n");
}

void Logger::error(const std::string &message)
{
	mBuffer->insert("ERR  " + message + "\n");
}

void Logger::critical(const std::string &message)
{
	mBuffer->insert("CRI  " + message + "\n");
}

////////////////////////////////////////////////////////////////////////
// Getters
////////////////////////////////////////////////////////////////////////

Buffer *Logger::getBuffer()
{
	return mBuffer;
}

////////////////////////////////////////////////////////////////////////
// Setters
////////////////////////////////////////////////////////////////////////

void Logger::setBuffer(Buffer *buffer)
{
	mBuffer = buffer;
}

////////////////////////////////////////////////////////////////////////
// Private
////////////////////////////////////////////////////////////////////////

Logger::Logger()
{
	mBuffer = nullptr;
}
