#pragma once

#include "asset/Asset.h"

namespace BHive
{
	DECLARE_CLASS()
	class PhysicsMaterial : public Asset
	{
	public:
		PhysicsMaterial() = default;

		DECLARE_PROPERTY()
		float mFrictionCoefficient = 1.0f;

		DECLARE_PROPERTY()
		float mBounciness = 0.0f;

		DECLARE_PROPERTY()
		float mMassDensity = 1.0f;

		DECLARE_FUNCTION()
		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		DECLARE_FUNCTION()
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Asset)
	};

} // namespace BHive