#pragma once

#include "asset/Asset.h"
#include "core/EnumAsByte.h"
#include "gfx/Color.h"
#include "gfx/Texture.h"

namespace BHive
{
	class Shader;
	class Texture;

	class BHIVE_API Material : public Asset
	{
	public:
		struct TextureSlot
		{
			uint32_t Binding;
			Ref<Texture> Texture;

			template <typename A>
			void Serialize(A &ar)
			{
				ar(MAKE_NVP("Binding", Binding), MAKE_NVP("Texture", TAssetHandle(Texture)));
			};
		};

	public:
		using TextureSlots = std::unordered_map<std::string, TextureSlot>;

	public:
		Material(const Ref<Shader> &shader);

		virtual void Submit(const Ref<Shader> &shader);

		virtual void SetTexture(const char *name, const Ref<Texture> &texture);

		virtual Ref<Shader> GetShader() const;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		virtual void Save(cereal::JSONOutputArchive &ar) const;

		virtual void Load(cereal::JSONInputArchive &ar);

		void AddTextureSlot(const std::string &name, uint32_t binding);

		REFLECTABLEV(Asset)

	protected:
		TextureSlots mTextures;
		Ref<Shader> mShader;
	};

	REFLECT_EXTERN(Material);

} // namespace BHive