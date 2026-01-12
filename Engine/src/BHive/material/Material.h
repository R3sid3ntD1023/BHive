#pragma once

#include "asset/Asset.h"
#include "core/EnumAsByte.h"
#include "gfx/Color.h"
#include "gfx/DescriptorBuilder.h"
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

		using TextureSlots = std::unordered_map<std::string, TextureSlot>;

	public:
		Material(const Ref<Shader> &shader);
		virtual ~Material() { DestroyDescriptorResources(); }

		virtual void Submit(const Ref<Shader> &shader);

		virtual void SetTexture(const char *name, const Ref<Texture> &texture);

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		// virtual Ref<Material> Clone() const { return nullptr; }

		void AddTextureSlot(const std::string &name, uint32_t binding);

		virtual Ref<Shader> GetShader() const;

		virtual bool ShouldCastShadows() const { return true; }

		const vk::raii::DescriptorSets &GetDescriptorSets() const { return mDescriptorSets; }

		const Ref<FDescriptorSetLayout> &GetDescriptorSetLayout() const { return mDescriptorSetLayout; }

		REFLECTABLEV(Asset)

	private:
		void CreateDescriptorResources();

		void DestroyDescriptorResources();

		void UpdateDescriptorResources();

	protected:
		TextureSlots mTextures;

		Ref<Shader> mShader;

	private:
		Ref<FDescriptorSetLayout> mDescriptorSetLayout;
		Ref<FDescriptorPool> mDescriptorPool;
		vk::raii::DescriptorSets mDescriptorSets = VK_NULL_HANDLE;
	};

	REFLECT_EXTERN(Material);

} // namespace BHive