#pragma once

#include "asset/Asset.h"
#include "serialization/Serialization.h"

namespace BHive
{
	class BHIVE PhysicsMaterial : public Asset, public ISerializable
	{
	public:
		PhysicsMaterial() = default;

		float mFrictionCoefficient = 1.0f;
		float mBounciness = 0.0f;
		float mMassDensity = 1.0f;


		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		ASSET_CLASS(PhysicsMaterial);
		REFLECTABLE()

	};

	REFLECT_EXTERN(PhysicsMaterial)

}