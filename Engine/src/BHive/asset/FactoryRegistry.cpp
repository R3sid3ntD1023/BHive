#include "FactoryRegistry.h"
#include "reflection/Reflection.h"
#include "asset/Factory.h"

namespace BHive
{
	FactoryRegistry::FactoryRegistry()
	{
		auto derived_fentityies = rttr::type::get<Factory>().get_derived_classes();
		for (auto &Factory_type : derived_fentityies)
		{
			auto asset_type = Factory_type.get_metadata("Type").get_value<AssetType>();
			auto extensions = Factory_type.get_metadata("Extensions").get_value<FAssetExtensions>();
			auto factory = Factory_type.create().get_value<Ref<Factory>>();

			Register(asset_type.get_name().data(), asset_type, extensions, factory);
		}
	}

	void FactoryRegistry::Register(const char *name, const AssetType &type_id, const FAssetExtensions &extensions, const Ref<Factory> &Factory)
	{
		ASSERT(!mRegisteredTypes.contains(type_id));

		mRegisteredTypes[type_id] = {name, extensions, Factory};
	}

	Ref<Factory> FactoryRegistry::Get(const AssetType &type_id) const
	{
		ASSERT(mRegisteredTypes.contains(type_id));

		return mRegisteredTypes.at(type_id).mFactory;
	}

	const AssetType &FactoryRegistry::GetTypeFromExtension(const std::string &ext) const
	{
		for (auto &extension : mRegisteredTypes)
		{
			if (extension.second.mExtensions.Contains(ext))
				return extension.first;
		}

		return InvalidType;
	}
}