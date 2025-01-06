#pragma once

#include "core/Core.h"
#include "AssetExtensions.h"
#include "asset/AssetType.h"

namespace BHive
{
	class Factory;

	class FactoryRegistry
	{
	public:
		struct TypeInfo
		{
			const char *mName;
			FAssetExtensions mExtensions;
			Ref<Factory> mFactory;

			operator bool() const { return mFactory != nullptr; }
		};

	public:
		FactoryRegistry();

		template <typename T, typename TFactory, typename = std::enable_if<std::is_base_of_v<Factory, TFactory>>>
		void Register(const FAssetExtensions &extensions)
		{
			auto type = AssetType::get<T>();
			Register(type.get_name().data(), type, extensions, CreateRef<TFactory>());
		}

		template <typename T>
		Ref<Factory> Get() const
		{
			return Get(AssetType::get<T>());
		}

		void Register(const char *name, const AssetType &type_id, const FAssetExtensions &extensions, const Ref<Factory> &Factory);

		Ref<Factory> Get(const AssetType &type_id) const;

		const AssetType &GetTypeFromExtension(const std::string &ext) const;

		static FactoryRegistry &Get()
		{
			static FactoryRegistry instance;
			return instance;
		}

		template <typename T>
		const TypeInfo &GetTypeInfo() const
		{
			static TypeInfo info;

			auto type = AssetType::get<T>();
			if (mRegisteredTypes.contains(type))
			{
				return mRegisteredTypes.at(type);
			}

			return info;
		}

		const std::unordered_map<AssetType, TypeInfo> &GetRegisteredFentityies() const { return mRegisteredTypes; }

	private:
		std::unordered_map<AssetType, TypeInfo> mRegisteredTypes;
	};

}