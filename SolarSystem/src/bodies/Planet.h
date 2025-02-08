#pragma once

#include "CelestrialBody.h"
#include "PlanetData.h"

namespace BHive
{
	class StaticMesh;
	class Texture;

} // namespace BHive
namespace SolarSystem
{

	struct Planet : public CelestrialBody
	{
		Planet();

		virtual void Update(const Ref<BHive::Shader> &shader, float dt) override;

		void Initialize(const PlanetData &data);

		void Save(cereal::JSONOutputArchive &ar) const;

		void Load(cereal::JSONInputArchive &ar);

		REFLECTABLEV(CelestrialBody)

	private:
		PlanetData mData;

		Ref<BHive::Texture> mTexture;
		static inline Ref<BHive::StaticMesh> mSphere;
	};

} // namespace SolarSystem

REFLECT(SolarSystem::Planet)
{
	BEGIN_REFLECT(SolarSystem::Planet, "Planet")
	REFLECT_CONSTRUCTOR();
}