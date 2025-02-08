#pragma once

#include <core/reflection/Reflection.h>
#include <core/serialization/Serialization.h>
#include <math/Transform.h>

namespace BHive
{
	class Shader;
}

namespace SolarSystem
{
	struct CelestrialBody
	{
		virtual void Update(const Ref<BHive::Shader> &shader, float) {}

		const BHive::FTransform &GetTransform() const { return mTransform; }

		virtual void Save(cereal::JSONOutputArchive &ar) const { ar(mTransform); }

		virtual void Load(cereal::JSONInputArchive &ar) { ar(mTransform); }

		REFLECTABLEV()

	protected:
		BHive::FTransform mTransform;
	};

} // namespace SolarSystem

REFLECT(SolarSystem::CelestrialBody)
{
	BEGIN_REFLECT(SolarSystem::CelestrialBody, "CelestrialBody");
}