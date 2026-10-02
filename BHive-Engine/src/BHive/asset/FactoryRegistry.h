#pragma once

#include "Factory.h"
#include "core/Core.h"

namespace BHive
{
	class BHIVE_API FactoryRegistry
	{
	public:
		FactoryRegistry();

		template <typename TFactory, typename = std::enable_if<std::is_base_of_v<Factory, TFactory>>>
		TFactory *Register()
		{
			auto factory = CreateRef<TFactory>();

			auto ptr = factory.get();

			mFactories.emplace_back(std::move(factory));

			mTypeMap.emplace(ptr->GetAssetType().get_id(), ptr);

			return ptr;
		}

		void Register(Factory *factory, std::initializer_list<std::string> exts);

		Factory *Find(std::string_view extension) const;

		Factory *Find(const rttr::type &type) const;

		const auto &GetFactories() const { return mFactories; }

		static FactoryRegistry &Get()
		{
			static FactoryRegistry instance;
			return instance;
		}

	private:
		std::vector<Ref<Factory>> mFactories;
		std::unordered_map<std::string, Factory *> mExtensionMap;
		std::unordered_map<rttr::type::type_id, Factory *> mTypeMap;
	};

} // namespace BHive