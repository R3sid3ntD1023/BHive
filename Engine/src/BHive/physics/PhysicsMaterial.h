#pragma once

#include "asset/Asset.h"

namespace BHive
{
	class BHIVE PhysicsMaterial : public Asset
	{
	public:
		PhysicsMaterial() = default;

		float mFrictionCoefficient = 1.0f;
		float mBounciness = 0.0f;
		float mMassDensity = 1.0f;

		virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

		REFLECTABLEV(Asset)
	};

	REFLECT_EXTERN(PhysicsMaterial)

} // namespace BHive