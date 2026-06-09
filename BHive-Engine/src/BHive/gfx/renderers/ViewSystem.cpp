#include "ViewSystem.h"

namespace BHive
{
	void ViewSystem::BeginFrame()
	{
		mViews.clear();
	}

	void ViewSystem::EndFrame()
	{

	}

	FView & ViewSystem::CreateMainView()
	{
		mViews.emplace_back();
		return mViews.back();
	}

	const FView & ViewSystem::GetMainView() const
	{
		return mViews.front();
	}

}