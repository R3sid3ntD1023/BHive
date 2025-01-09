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


		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		ASSET_CLASS(PhysicsMaterial);
		REFLECTABLEV(Asset)

	};

	REFLECT_EXTERN(PhysicsMaterial)

}