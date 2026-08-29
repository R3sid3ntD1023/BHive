#pragma once

#include "asset/Asset.h"
#include "core/EnumAsByte.h"
#include "gfx/Color.h"
#include "gfx/Texture.h"
#include "BackendMaterial.h"

namespace BHive
{
	class Texture;

	class BHIVE_API Material : public Asset, public IMaterial
	{
	public:
		enum class ESurfaceType : uint8_t
		{
			Opaque,
			Transparent,
			Additive
		};

		enum EFlags : uint32_t
		{
			None,
			DoubleSided = BIT(2)
		};

	public:
		Material(const std::string &shaderProgramName);

		virtual ~Material() = default;

		IMaterial &SetParam(const std::string &name, const MaterialParam &param) & override;

		IMaterial &SetTexture(const std::string &name, const FTextureBinding &texture) & override;

		IMaterial &SetSurfaceType(ESurfaceType surfaceType);

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		// virtual Ref<Material> Clone() const { return nullptr; }

		virtual bool ShouldCastShadows() const { return true; }

		Ref<IMaterialBackendInterface> GetNative() const override { return mBackendMaterial; }

		MaterialSnapshot CreateSnapshot() const override;

		ESurfaceType GetSurfaceType() const { return mSurfaceType; }

		virtual bool IsTransparent() const { return GetSurfaceType() == ESurfaceType::Transparent; }

		virtual EFlags GetFlags() const { return EFlags::None; }

		REFLECTABLEV(Asset)

	private:
		void InitFromReflection();

	protected:
		std::string mShaderProgramName;

		ESurfaceType mSurfaceType = ESurfaceType::Opaque;

		std::unordered_map<std::string, FTextureBinding> mTextures;

		std::unordered_map<std::string, MaterialParam> mParams;

		mutable Ref<IMaterialBackendInterface> mBackendMaterial;

	private:
	};

	REFLECT_EXTERN(Material);

} // namespace BHive