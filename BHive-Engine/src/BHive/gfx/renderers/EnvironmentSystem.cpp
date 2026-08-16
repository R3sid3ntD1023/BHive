#include "EnvironmentSystem.h"
#include "RenderGraphScheduler.h"
#include "Renderer.h"

namespace BHive
{
	EnvironmentSystem::EnvironmentSystem()
	{
		mPMREM.Initialize();
		mBRDFLut = BRDFLUTGenerator::GenerateBRDFLUTMap();
	}

	void EnvironmentSystem::SetHDR(const Ref<Texture2D> &hdr)
	{
		mPendingHDR = hdr;
		mDirty = true;
	}

	void EnvironmentSystem::Update()
	{
		if (!mDirty || !mPendingHDR)
			return;

		mMaps = mPMREM.GenerateEnvironmentMaps(mPendingHDR);

		mPendingHDR.reset();
		mDirty = false;
	}
} // namespace BHive