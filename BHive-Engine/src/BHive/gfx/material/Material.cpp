#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "Material.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"

namespace BHive
{
	Material::Material(const std::string &shaderProgramName)
		: mShaderProgramName(shaderProgramName)
	{
		InitFromReflection();
	}

	IMaterial &Material::SetParam(const std::string &name, const MaterialParam &param) &
	{
		mParams[name] = param;
		if (mBackendMaterial)
			mBackendMaterial->SetParam(name, param);

		return *this;
	}

	IMaterial &Material::SetTexture(const std::string &name, const FTextureBinding &texture) &
	{
		mTextures[name] = texture;
		if (mBackendMaterial)
			mBackendMaterial->SetTexture(name, texture);

		return *this;
	}

	IMaterial &Material::SetSurfaceType(ESurfaceType surfaceType)
	{
		mSurfaceType = surfaceType;
		return *this;
	}

	MaterialSnapshot Material::CreateSnapshot() const
	{
		if (!mBackendMaterial)
			mBackendMaterial = IMaterialBackendInterface::Create(mShaderProgramName);

		for (auto &[name, value] : mParams)
			mBackendMaterial->SetParam(name, value);

		FTextureBinding binding{};
		for (auto &[name, tex] : mTextures)
		{
			binding = tex;
			if (!binding.TextureRef)
				binding.TextureRef = Renderer::Get().GetWhiteTexture();

			mBackendMaterial->SetTexture(name, binding);
		}

		return mBackendMaterial->CreateSnapshot();
	}

	void Material::InitFromReflection()
	{
		auto program = Renderer::Get().GetShaderManager().Get(mShaderProgramName);
		const auto &merged = program->GetMergedRefl();

		if (!merged.Sets.contains(1))
			return;

		const auto &matSet = merged.Sets.at(1);

		for (auto &[name, ubo] : matSet.UniformBuffers)
			mParams[name] = MaterialParam(ubo.Size);

		for (auto &[name, ssbo] : matSet.StorageBuffers)
			mParams[name] = MaterialParam(ssbo.Size);

		for (auto &pc : merged.PushConstants)
			for (auto &mem : pc.Members)
				mParams[mem.first] = MaterialParam(mem.second.Size);

		for (auto &[name, saampler] : matSet.Samplers)
			mTextures[name] = {nullptr, 0};
	}

	void Material::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);

		ar(mTextures, mParams);
	}

	void Material::Load(cereal::BinaryInputArchive &ar)
	{

		Asset::Load(ar);

		ar(mTextures, mParams);
	}

	REFLECT(FTextureBinding)
	{
		BEGIN_REFLECT(FTextureBinding)
		REFLECT_PROPERTY("Texture", TextureRef)
		REFLECT_PROPERTY("MipLevel", BaseMipLevel)
		REFLECT_PROPERTY("ArrayLayer", BaseArrayLayer);
	}

	REFLECT(MaterialParam)
	{
		BEGIN_REFLECT(MaterialParam);
	}

	REFLECT(Material)
	{
		BEGIN_REFLECT(Material)
		REFLECT_PROPERTY("Textures", mTextures);

		rttr::type::register_wrapper_converter_for_base_classes<Ref<Material>>();
	}

} // namespace BHive