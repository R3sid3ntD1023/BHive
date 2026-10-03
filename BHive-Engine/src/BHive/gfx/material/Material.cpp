#include "Material.h"
#include "core/utils/Hash.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/shader/Shader.h"

namespace BHive
{
	Material::Material(const std::string &shaderProgramName)
		: mShaderProgramName(shaderProgramName)
	{
		mBackendMaterial = IMaterialBackendInterface::Create(mShaderProgramName);
	}

	IMaterial &Material::SetParam(const std::string &name, const MaterialParam &param) &
	{
		mBackendMaterial->SetParam(name, param);
		return *this;
	}

	IMaterial &Material::SetTexture(const std::string &name, const TextureBinding &texture) &
	{
		auto t = texture;
		if (!t.Texture)
			t.Texture = Renderer::Get().GetWhiteTexture();

		mBackendMaterial->SetTexture(name, t);
		return *this;
	}

	IMaterial &Material::SetSurfaceType(ESurfaceType surfaceType)
	{
		mSurfaceType = surfaceType;
		return *this;
	}

	MaterialSnapshot Material::CreateSnapshot() const
	{
		return mBackendMaterial->CreateSnapshot();
	}

	MaterialSnapshot Material::CreateSnapshot(EShaderMeshType meshType) const
	{
		auto snapshot = mBackendMaterial->CreateSnapshot();
		if (meshType == EShaderMeshType::None)
			return snapshot;

		auto definitions = GetShaderDefinitions();
		switch (meshType)
		{
		case EShaderMeshType::Static:
			definitions.emplace_back("MESH_STATIC");
			break;
		case EShaderMeshType::Skeletal:
			definitions.emplace_back("MESH_SKELETAL");
			definitions.emplace_back("SKINNING");
			break;
		case EShaderMeshType::None:
			break;
		}

		if (!definitions.empty())
		{
			auto variant = ShaderManager::GetVariant(mShaderProgramName, std::move(definitions));
			ASSERT(variant, "Failed to load shader variant for material {}", mShaderProgramName);
			snapshot.Shader = variant;
		}
		return snapshot;
	}

	void Material::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
	}

	void Material::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
	}

	REFLECT(TextureBinding)
	{
		BEGIN_REFLECT(TextureBinding)
		REFLECT_PROPERTY("Texture", Texture)
		REFLECT_PROPERTY("MipLevel", BaseMipLevel)
		REFLECT_PROPERTY("ArrayLayer", BaseArrayLayer);
	}

	REFLECT(MaterialParam)
	{
		BEGIN_REFLECT(MaterialParam);
	}

	REFLECT(Material)
	{
		BEGIN_REFLECT(Material);

		rttr::type::register_wrapper_converter_for_base_classes<Ref<Material>>();
	}

} // namespace BHive