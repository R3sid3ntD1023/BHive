#include "Planet.h"
#include <gfx/RenderCommand.h>
#include <importers/TextureImporter.h>
#include <mesh/MeshImporter.h>
#include <mesh/primitives/Sphere.h>

namespace SolarSystem
{
	Planet::Planet(const PlanetData &data)
		: mData(data)
	{
		if (!mSphere)
		{
			mSphere = CreateRef<BHive::PSphere>(1.f, 32, 32);
		}

		if (!mWhiteTexture)
		{
			uint32_t white = 0xFFFFFF;
			BHive::FTextureSpecification specs;
			specs.mFormat = BHive::EFormat::RGB8;
			specs.mChannels = 3;
			mWhiteTexture = BHive::Texture2D::Create(&white, 1, 1, specs);
		}

		if (!data.mTexturePath.empty())
		{
			mTexture = BHive::TextureImporter::Import(data.mTexturePath);
		}
	}

	void Planet::Update(float dt)
	{
		auto texture = mTexture;
		if (!texture)
		{
			texture = mWhiteTexture;
		}

		texture->Bind();

		BHive::RenderCommand::DrawElements(BHive::EDrawMode::Triangles, *mSphere->GetVertexArray());
	}
} // namespace SolarSystem