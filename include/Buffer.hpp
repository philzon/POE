#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <exception>
#include <fstream>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>

struct Cursor
{
	unsigned int x;
	unsigned int y;
};

class Buffer
{
public:
	Buffer();
	Buffer(const std::vector<std::string> &lines);
	Buffer(const Buffer &other);
	Buffer &operator=(const Buffer &other);
	virtual ~Buffer();

	bool read(const std::string &path);
	bool read(const std::string &path, bool append);
	bool write(const std::string &path);

	void clear();
	void del();
	void down();
	void erase();
	void insert(char ch);
	void insert(const std::string &text);
	void left();
	void right();
	void up();

	// Getters.
	bool isDirty() const;
	bool isInsertMode() const;
	bool isMultilineMode() const;
	bool isReadOnlyMode() const;
	char get(unsigned int line, unsigned int column) const;
	unsigned int getColumns(unsigned int line) const;
	std::string getError() const;
	unsigned int getLines() const;
	Cursor getCursor() const;
	std::string getLine(unsigned int line) const;
	std::string getTitle() const;
	unsigned int getWrap() const;
	bool hasFlags() const;
	std::string toString() const;

	// Setters.
	void setCursor(unsigned int line, unsigned int column);
	void setCursor(const Cursor &cursor);
	void setDirty(bool state);
	void setInsertMode(bool state);
	void setMultilineMode(bool state);
	void setReadOnlyMode(bool state);
	void setTitle(const std::string &title);
	void setWrap(unsigned int amount);

private:
	void newline();
	void removeline();

	std::string mError;

	// Buffer states/modes.
	bool mDirty;
	bool mInsertMode;
	bool mMultilineMode;
	bool mReadOnlyMode;

	// Cursor.
	Cursor mCursor;
	unsigned int mPreviousColumn;
	unsigned int mWrap;

	// Buffer information.
	std::string mTitle;
	std::vector<std::string> mLines;
};

#endif // BUFFER_HPP
