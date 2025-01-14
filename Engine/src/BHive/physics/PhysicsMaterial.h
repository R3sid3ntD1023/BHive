#pragma once

#include "asset/Asset.h"
#include "serialization/Serialization.h"

namespace BHive
{
	class BHIVE PhysicsMaterial : public Asset
	{
	public:
		PhysicsMaterial() = default;

		float mFrictionCoefficient = 1.0f;
		float mBounciness = 0.0f;
		float mMassDensity = 1.0f;

		void Serialize(StreamWriter &ar) const override;
		void Deserialize(StreamReader &ar) override;

		REFLECTABLEV(Asset)
	};

	REFLECT_EXTERN(PhysicsMaterial)

} // namespace BHive