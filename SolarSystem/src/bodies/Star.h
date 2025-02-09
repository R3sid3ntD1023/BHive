#pragma once

#include "CelestrialBody.h"
#include <gfx/Color.h>

namespace BHive
{
	class StaticMesh;

} // namespace BHive

namespace SolarSystem
{

	class Star : public CelestrialBody
	{
	public:
		float Brightness = 1.f;
		float Radius = 20.f;
		BHive::Color Color = {1.f, 1.f, 1.f, 1.f};
		BHive::Color Emission = {10.f, 5.f, .5f, 1.f};

	public:
		Star();

		virtual void OnUpdate(const Ref<BHive::Shader> &shader, float);

		void Save(cereal::JSONOutputArchive &ar) const;

		void Load(cereal::JSONInputArchive &ar);

		REFLECTABLEV(CelestrialBody)

	private:
		static inline Ref<BHive::StaticMesh> mSphere;
	};

} // namespace SolarSystem

REFLECT(SolarSystem::Star)
{
	BEGIN_REFLECT(SolarSystem::Star, "Star")
	REFLECT_CONSTRUCTOR();
}