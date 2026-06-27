#include "EnvironmentSystem.h"
#include "RenderGraphScheduler.h"
#include "Renderer.h"

namespace BHive
{
	EnvironmentSystem::EnvironmentSystem(GlobalResources& globals)
		: mGlobals(globals)
	{
		mPMREM.Initialize();

		mGlobals.Register("EnvironmentCubeMap", mPMREM.GetEnvironmentCube());
		mGlobals.Register("EnvironmentIrradiance", mPMREM.GetIrradiance());
		mGlobals.Register("EnvironmentPreFilter", mPMREM.GetPreFilter());
	}

	void EnvironmentSystem::BeginFrame()
	{

	}

	void EnvironmentSystem::SetHDR(const Ref<Texture2D>& hdr)
	{
		mPendingHDR = hdr;
		mDirty = true;
	}

	void EnvironmentSystem::Update(RenderGraphScheduler& scheduler)
	{
		if (!mDirty || !mPendingHDR)
			return;

		mMaps = mPMREM.GenerateEnvironmentMaps(mPendingHDR);

		if (mMaps.IsValid())
		{
			mGlobals.Register("EnvironmentCubeMap", mMaps.Environment);
			mGlobals.Register("EnvironmentIrradiance", mMaps.Irradiance);
			mGlobals.Register("EnvironmentPreFilter", mMaps.PreFilter);
		}

		mPendingHDR.reset();
		mDirty = false;
	}
}