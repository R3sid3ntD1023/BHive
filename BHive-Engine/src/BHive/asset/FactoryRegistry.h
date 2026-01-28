#pragma once

#include "core/Core.h"

namespace BHive
{
	class Factory;

	class BHIVE_API FactoryRegistry
	{
		using factories = std::vector<Ref<Factory>>;

	public:
		FactoryRegistry();

		template <typename TFactory, typename = std::enable_if<std::is_base_of_v<Factory, TFactory>>>
		void Register()
		{
			Register(CreateRef<TFactory>());
		}

		void Register(const Ref<Factory> &Factory);

		Ref<Factory> Get(const std::string &extension) const;

		static FactoryRegistry &Get()
		{
			static FactoryRegistry instance;
			return instance;
		}

		const factories &GetRegisteredFactories() const { return mRegisteredFactories; }

	private:
		// extension - factory
		factories mRegisteredFactories;
	};

} // namespace BHive