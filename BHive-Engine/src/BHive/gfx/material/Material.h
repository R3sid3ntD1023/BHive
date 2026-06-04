#pragma once

#include "asset/Asset.h"
#include "core/EnumAsByte.h"
#include "gfx/Color.h"
#include "gfx/Texture.h"
#include "BackendMaterial.h"

namespace BHive
{
	class Pipeline;
	class Texture;

	struct TextureSlot
	{
		Ref<Texture> Texture;

		uint32_t MipLevel = 0;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(TAssetHandle(Texture));
		};
	};


	using TextureSlotMap = std::unordered_map<std::string, TextureSlot>;

	class BHIVE_API Material : public Asset
	{
	public:
		Material() = default;

		virtual ~Material() = default;

		void SetPipeline(Pipeline *pipeline);

		virtual void Submit(Pipeline* pipeline = nullptr);

		virtual void SetTexture(const char *name, const Ref<Texture> &texture, uint32_t mip = 0);

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		// virtual Ref<Material> Clone() const { return nullptr; }

		virtual bool ShouldCastShadows() const { return true; }

		template<typename T>
		void Set(const std::string &name, const T &val);

		REFLECTABLEV(Asset)

	private:
		void BuildSlotsForPipeline(Pipeline* pipeline);

	protected:

		std::unordered_map<std::string, TextureSlot> mUserTextureSlots;

		std::unordered_map<Pipeline*, TextureSlotMap> mSlotsPerPipeline;

		Pipeline* mPipeline = nullptr;

		Scope<IMaterialBackendInterface> mBackendMaterial;

	private:
	};

	template <typename T>
	inline void Material::Set(const std::string &name, const T &val)
	{
		mBackendMaterial->Set(name, &val, sizeof(T));
	}

	REFLECT_EXTERN(Material);

} // namespace BHive