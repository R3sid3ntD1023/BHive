#pragma once

#include "PlanetData.h"

namespace BHive
{
	class StaticMesh;
	class Texture;

} // namespace BHive
namespace SolarSystem
{

	struct Planet
	{
		Planet(const PlanetData &data);

		virtual void Update(float dt);

		const BHive::FTransform &GetTransform() const { return mData.mTransform; }

	private:
		PlanetData mData;

		Ref<BHive::Texture> mTexture;
		static inline Ref<BHive::Texture> mWhiteTexture;
		static inline Ref<BHive::StaticMesh> mSphere;
	};
} // namespace SolarSystem