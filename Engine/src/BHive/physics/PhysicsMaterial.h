#pragma once

#include "asset/Asset.h"

namespace BHive
{
	REFLECT_CLASS()
	class PhysicsMaterial : public Asset
	{
	public:
		PhysicsMaterial() = default;

		REFLECT_PROPERTY()
		float mFrictionCoefficient = 1.0f;

		REFLECT_PROPERTY()
		float mBounciness = 0.0f;

		REFLECT_PROPERTY()
		float mMassDensity = 1.0f;

		REFLECT_FUNCTION()
		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		REFLECT_FUNCTION()
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Asset)
	};

} // namespace BHive