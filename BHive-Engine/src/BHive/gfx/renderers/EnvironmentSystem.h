#pragma once

#include "PMREMGenerator.h"
#include "gfx/GlobalResources.h"

namespace BHive
{
	class RenderGraphScheduler;

	class EnvironmentSystem
	{
	public:
		EnvironmentSystem(GlobalResources &globals);

		void BeginFrame();
		void SetHDR(const Ref<Texture2D> &hdr);
		void Update(RenderGraphScheduler &scheduler);

		const PMREMResult &GetCurrentMaps() const { return mMaps; }

	private:
		GlobalResources &mGlobals;
		PMREMGenerator mPMREM;

		Ref<Texture2D> mPendingHDR;
		bool mDirty = false;

		PMREMResult mMaps;
	};
}