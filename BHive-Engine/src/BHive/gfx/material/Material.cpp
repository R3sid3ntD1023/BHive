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