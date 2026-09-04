#pragma once

#include "PMREMGenerator.h"

namespace BHive
{
	class EnvironmentSystem
	{
	public:
		EnvironmentSystem();

		void SetHDR(Texture2DPtr hdr);

		void Update();

		const PMREMResult &GetCurrentMaps() const { return mMaps; }

		Texture2DPtr GetBRDFLUT() const { return mBRDFLut; }

	private:
		Texture2DPtr mPendingHDR;
		PMREMResult mMaps;
		Texture2DPtr mBRDFLut;
		bool mDirty = false;
	};
} // namespace BHive