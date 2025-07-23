#pragma once

#include "core/Core.h"
#include "core/UUID.h"
#include "core/reflection/Reflection.h"

namespace BHive
{
	class Asset;

	struct BHIVE_API FAssetContextMenu
	{
		virtual ~FAssetContextMenu() = default;

		virtual void OnAssetContext(const UUID &handle) = 0;
		virtual void OnAssetOpen(const UUID &handle) = 0;

		REFLECTABLEV()
	};

	REFLECT(FAssetContextMenu)
	{
		BEGIN_REFLECT(FAssetContextMenu);
	}

	template <typename T>
	struct BHIVE_API AssetContextMenuBuilder
	{
		AssetContextMenuBuilder(const std::string &className)
			: mClass(rttr::registration::class_<T>(className))
		{
			mClass.constructor<>()(rttr::policy::ctor::as_std_shared_ptr);
		}

		template <typename... Types>
		AssetContextMenuBuilder &RegisterSupportedTypes()
		{
			mSupportedTypes = {rttr::type::get<Types>()...};
			mClass(META_DATA("SupportedTypes", mSupportedTypes));
			return *this;
		}

	private:
		rttr::registration::class_<T> mClass;
		std::unordered_set<rttr::type> mSupportedTypes;
	};

} // namespace BHive

#define REFLECT_ASSET_MENU(cls, ...)                         \
	REFLECT(cls)                                             \
	{                                                        \
		::BHive::AssetContextMenuBuilder<cls> builder(#cls); \
		builder.RegisterSupportedTypes<__VA_ARGS__>();       \
	}
