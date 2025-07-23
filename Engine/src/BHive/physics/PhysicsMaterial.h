#pragma once

#include "asset/Asset.h"

namespace BHive
{
	class BHIVE_API PhysicsMaterial : public Asset
	{
	public:
		PhysicsMaterial() = default;

		float FrictionCoefficient = 1.0f;

		float Bounciness = 0.0f;

		float MassDensity = 1.0f;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Asset)
	};

	REFLECT_EXTERN(PhysicsMaterial)

} // namespace BHive