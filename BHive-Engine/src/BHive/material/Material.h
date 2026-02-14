#pragma once

#include "asset/Asset.h"
#include "core/EnumAsByte.h"
#include "gfx/Color.h"
#include "gfx/Texture.h"

namespace BHive
{
	class Pipeline;
	class IMaterialBackendInterface;
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

		using TextureSlots = std::unordered_map<std::string, TextureSlot>;

	public:
		Material(Ref<Pipeline> pipeline);

		virtual ~Material() = default;

		virtual void Submit(Ref<Pipeline> pipeline = nullptr);

		virtual void SetTexture(const char *name, const Ref<Texture> &texture);

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		// virtual Ref<Material> Clone() const { return nullptr; }

		void AddTextureSlot(const std::string &name, uint32_t binding);

		virtual bool ShouldCastShadows() const { return true; }

		REFLECTABLEV(Asset)

	private:
		void UpdateTextureSlots();

		void CreateBackendMaterial();

	protected:
		TextureSlots mTextures;

		Ref<Pipeline> mPipeline;

		Ref<IMaterialBackendInterface> mBackendMaterial;

	private:
	};

	REFLECT_EXTERN(Material);

} // namespace BHive