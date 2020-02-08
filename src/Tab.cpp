#include "Tab.hpp"

Tab::Tab()
{
	mCurrentView = 0;
}

Tab::~Tab()
{}

void Tab::render(unsigned int width, unsigned int height)
{
	if (mViews.empty())
		return;

	// TODO: Draw other items?
	mViews.at(mCurrentView).render(0, 0, width, height);
}

void Tab::add(const View &view)
{
	mViews.push_back(view);
	mCurrentView = mViews.size() - 1;
}

void Tab::next()
{
	if (mCurrentView < mViews.size() - 1)
		++mCurrentView;
	else
		mCurrentView = 0;
}

void Tab::previous()
{
	if (mCurrentView > 0)
		--mCurrentView;
	else
		mCurrentView = mViews.size() - 1;
}

void Tab::remove()
{
	if (!mViews.empty())
		mViews.erase(mViews.begin() + mCurrentView);

	if (mCurrentView > mViews.size() - 1)
		mCurrentView = mViews.size() - 1;
}

////////////////////////////////////////////////////////////////////////
// Getters
////////////////////////////////////////////////////////////////////////

Buffer *Tab::getBuffer()
{
	if (mViews.empty())
		return nullptr;

	return mViews.at(mCurrentView).getBuffer();
}

unsigned int Tab::size() const
{
	return mViews.size();
}
