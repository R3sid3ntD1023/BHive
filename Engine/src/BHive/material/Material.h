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

	class Material : public Asset
	{
	public:
		using TextureSlots = std::unordered_map<std::string, uint32_t>;
		using Textures = std::unordered_map<std::string, Ref<Texture>>;

	public:
		Material();
		Material(const Ref<Shader> &shader);

		virtual void Submit(const Ref<UniformBuffer> &material_buffer) {};

		virtual void SetTexture(const char *name, const Ref<Texture> &texture);

		virtual Ref<Shader> GetShader() const;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Asset)

	protected:
		TextureSlots mTextureSlots;
		Textures mTextures;
		Ref<Shader> mShader;
	};

	REFLECT_EXTERN(Material);

} // namespace BHive