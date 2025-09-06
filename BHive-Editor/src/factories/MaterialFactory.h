#pragma once

#include "asset/Factory.h"

namespace BHive
{
	class StandardMaterialFactory : public Factory
	{
	public:
		virtual Ref<Asset> CreateNew() override;

		virtual bool CanCreateNew() const { return true; }

		virtual std::string GetDefaultAssetName() const { return "NewStandardMaterial"; }

		virtual std::string GetDisplayName() const { return "Standard Material"; }

		REFLECTABLEV(Factory)
	};

	class LambertMaterialFactory : public Factory
	{
	public:
		virtual Ref<Asset> CreateNew() override;

		virtual bool CanCreateNew() const { return true; }

		virtual std::string GetDefaultAssetName() const { return "NewLambertMaterial"; }

		virtual std::string GetDisplayName() const { return "Lambert Material"; }

		REFLECTABLEV(Factory)
	};

	class EmissiveMaterialFactory : public Factory
	{
	public:
		virtual Ref<Asset> CreateNew() override;

		virtual bool CanCreateNew() const { return true; }

		virtual std::string GetDefaultAssetName() const { return "NewEmissiveMaterial"; }

		virtual std::string GetDisplayName() const { return "Emissive Material"; }

		REFLECTABLEV(Factory)
	};

	REFLECT_EXTERN(StandardMaterialFactory)
	REFLECT_EXTERN(LambertMaterialFactory)
	REFLECT_EXTERN(EmissiveMaterialFactory)

} // namespace BHive
