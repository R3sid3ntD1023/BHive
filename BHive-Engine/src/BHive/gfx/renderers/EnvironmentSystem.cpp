#include "EnvironmentSystem.h"
#include "Renderer.h"
#include "RenderGraphScheduler.h"

namespace BHive
{
	EnvironmentSystem::EnvironmentSystem()
	{
		PMREMGenerator::Initialize();
		mBRDFLut = BRDFLUTGenerator::GenerateBRDFLUTMap();
	}

	void EnvironmentSystem::SetHDR(Ref<Texture2D> hdr)
	{
		mPendingHDR = hdr;
		mDirty = true;
	}

	void EnvironmentSystem::Update()
	{
		if (!mDirty || !mPendingHDR)
			return;

		mMaps = PMREMGenerator::GenerateEnvironmentMaps(mPendingHDR);

		mPendingHDR = {};
		mDirty = false;
	}
} // namespace BHive