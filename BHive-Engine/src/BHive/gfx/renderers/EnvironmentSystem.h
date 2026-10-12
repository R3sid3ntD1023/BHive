#pragma once

#include "PMREMGenerator.h"

namespace BHive
{
	class EnvironmentSystem
	{
	public:
		EnvironmentSystem();

		void SetHDR(Ref<Texture2D> hdr);

		void Update();

		const PMREMResult &GetCurrentMaps() const { return mMaps; }

		Ref<Texture2D> GetBRDFLUT() const { return mBRDFLut; }

	private:
		Ref<Texture2D> mPendingHDR;
		PMREMResult mMaps;
		Ref<Texture2D> mBRDFLut;
		bool mDirty = false;
	};
} // namespace BHive