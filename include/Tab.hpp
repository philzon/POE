#ifndef TAB_HPP
#define TAB_HPP

#include <vector>

#include "View.hpp"

class Tab
{
public:
	Tab();
	virtual ~Tab();

	void render(unsigned int width, unsigned int height);

	void add(const View &view);
	void remove();
	void next();
	void previous();

	// Getters.
	Buffer *getBuffer();
	unsigned int size() const;

private:
	std::vector<View> mViews;
	unsigned int mCurrentView;
};

#endif // TAB_HPP
