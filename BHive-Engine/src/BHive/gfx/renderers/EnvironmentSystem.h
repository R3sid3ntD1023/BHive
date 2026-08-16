#pragma once

#include "PMREMGenerator.h"

namespace BHive
{
	class EnvironmentSystem
	{
	public:
		EnvironmentSystem();

		void SetHDR(const Ref<Texture2D> &hdr);

		void Update();

		const PMREMResult &GetCurrentMaps() const { return mMaps; }

		Ref<Texture2D> GetBRDFLUT() const { return mBRDFLut; }

	private:
		PMREMGenerator mPMREM;
		Ref<Texture2D> mPendingHDR;
		PMREMResult mMaps;
		Ref<Texture2D> mBRDFLut;
		bool mDirty = false;
	};
} // namespace BHive