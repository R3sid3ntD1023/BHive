#pragma once

#include <core/Core.h>
#include <math/Transform.h>

namespace BHive
{
	class StaticMesh;
	class Texture;

} // namespace BHive
namespace SolarSystem
{
	struct PlanetData
	{
		PlanetData(const std::string &name, const BHive::FTransform &transform, const std::string &texture_path)
			: mName(name),
			  mTransform(transform),
			  mTexturePath(texture_path)
		{
		}
		std::string mName;
		BHive::FTransform mTransform;
		std::string mTexturePath;
	};

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