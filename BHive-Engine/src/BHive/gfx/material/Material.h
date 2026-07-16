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

	class BHIVE_API Material : public Asset, public IMaterial
	{
	public:
		enum class EShadingModel : uint8_t
		{
			Lambert,
			Emissive,
			Standard
		};

		enum class ESurfaceType : uint8_t
		{
			Opaque,
			Transparent,
			Additive
		};

		enum EFlags : uint32_t
		{
			HAS_NORMAL_MAP = BIT(1),
			DOUBLE_SIDED = BIT(2)
		};

	public:
		Material() = default;

		virtual ~Material() = default;

		void SetPipeline(Pipeline *pipeline);

		virtual void Submit(Pipeline *pipeline = nullptr);

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		// virtual Ref<Material> Clone() const { return nullptr; }

		virtual bool ShouldCastShadows() const { return true; }

		template <typename T>
		void Set(const std::string &name, const T &val);

		void SetTexture(const std::string &name, const Ref<Texture> &texture, uint32_t mip = 0);

		Ref<IMaterialBackendInterface> GetNative() const override { return mBackendMaterial; }

		MaterialSnapshot CreateSnapshot() const override { return mBackendMaterial->CreateSnapshot(); }

		Pipeline *GetPipeline() const override { return mPipeline; }

		REFLECTABLEV(Asset)

	private:
		void BuildSlotsForPipeline(Pipeline *pipeline);

		void UpdatePipeline();

	protected:
		EShadingModel mShadingModel;

		ESurfaceType mSurfaceType;

		std::unordered_map<std::string, TextureSlot> mUserTextureSlots;

		std::unordered_map<Pipeline *, TextureSlotMap> mSlotsPerPipeline;

		Pipeline *mPipeline = nullptr;

		Ref<IMaterialBackendInterface> mBackendMaterial;

	private:
	};

	template <typename T>
	inline void Material::Set(const std::string &name, const T &val)
	{
		mBackendMaterial->Set(name, &val, sizeof(T));
	}

	REFLECT_EXTERN(Material);

} // namespace BHive