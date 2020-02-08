#ifndef INI_HPP
#define INI_HPP

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>

class INI
{
public:
	INI();
	INI(const std::string& path);
	INI(const INI &other);
	INI &operator=(const INI &other);
	virtual ~INI();

	void clear();

	bool load(const std::string &file);
	bool save(const std::string &file);
	bool parse(const char *data, std::size_t size);
	bool parse(const std::vector<char> &data);
	bool parse(const std::vector<std::string> &lines);

	std::string getErrors() const;

	std::string getString(const std::string &section, const std::string &key, const std::string &def);
	int getInteger(const std::string &section, const std::string &key, int def);
	float getFloat(const std::string &section, const std::string &key, float def);
	bool getBoolean(const std::string &section, const std::string &key, bool def);

	void setValue(const std::string &section, const std::string &key, const std::string &value);
	void setValue(const std::string &section, const std::string &key, int value);
	void setValue(const std::string &section, const std::string &key, float value);
	void setValue(const std::string &section, const std::string &key, bool value);

	bool empty() const;
	unsigned int sectionCount() const;
	unsigned int keyCount() const;

protected:
	std::string mErrors;
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> mSections;
};

#endif // INI_HPP
