#pragma once

#include "Material.h"

namespace BHive
{
	class BHIVE_API StandardMaterial : public Material
	{
	public:
		enum EFlags : uint16_t
		{
			None = 0,
			CastShadows = BIT(0),
			ReceiveShadows = BIT(1),
			DiaElectric = BIT(2),
			Shadows = CastShadows | ReceiveShadows
		};

	public:
	public:
		StandardMaterial();

		StandardMaterial &SetAlbedo(FColor color);

		StandardMaterial &SetEmission(FColor color);

		StandardMaterial &SetMetalness(float metalness);

		StandardMaterial &SetRoughness(float roughness);

		StandardMaterial &SetOpacity(float opacity);

		StandardMaterial &SetDepthScale(float depthScale);

		StandardMaterial &SetTiling(glm::vec2 tiling);

		StandardMaterial &SetFlags(EFlags flags);

		IMaterial &SetTexture(const std::string &name, const FTextureBinding &texture) & override;

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		virtual bool ShouldCastShadows() const override;

		REFLECTABLEV(Material)

	private:
		FColor mAlbedo{1.0f, 1.0f, 1.0f, 1.0f};

		FColor mEmission{0.0f, 0.0f, 0.0f};

		float mMetalness{0.0f};

		float mRoughness{1.0f};

		float mOpacity{1.0f};

		float mDepthScale{1.0f};

		glm::vec2 mTiling{1.0f, 1.0f};

		TEnumAsByte<EFlags> mFlags = Shadows;
	};

	REFLECT_EXTERN(StandardMaterial)

} // namespace BHive