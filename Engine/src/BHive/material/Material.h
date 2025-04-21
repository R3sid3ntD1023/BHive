#pragma once

#include "asset/Asset.h"
#include "core/EnumAsByte.h"
#include "gfx/Color.h"
#include "gfx/Texture.h"


namespace BHive
{
	class Shader;
	class Texture;
	class UniformBuffer;

	struct BHIVE FTextureSlot
	{
		uint32_t mBinding = 0;
		Ref<Texture> mTexture;
	};

	class Material : public Asset
	{
	public:
		using TextureSlots = std::unordered_map<std::string, FTextureSlot>;
		using Textures = std::unordered_map<std::string, Ref<Texture>>;

	public:
		Material();
		Material(const Ref<Shader> &shader);

		virtual void Submit() const;

		virtual void SetTexture(const char *name, const Ref<Texture> &texture);

		virtual Ref<Shader> GetShader() const;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

	private:
		TextureSlots mTextureSlots;
		Textures mTextures;
		Ref<UniformBuffer> mMaterialData;
		Ref<Shader> mShader;
	};

} // namespace BHive