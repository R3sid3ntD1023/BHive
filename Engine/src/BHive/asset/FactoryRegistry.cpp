#include "FactoryRegistry.h"
#include "core/reflection/Reflection.h"
#include "asset/Factory.h"

namespace BHive
{
	FactoryRegistry::FactoryRegistry()
	{
		auto derived_types = rttr::type::get<Factory>().get_derived_classes();
		for (auto &type : derived_types)
		{
			auto factory = type.create();
			
			if (!factory)
			{
				LOG_WARN("No Factory found for type - {}", type);
				continue;
			}
				
			Register(factory.get_value<Ref<Factory>>());
		}
	}

	void FactoryRegistry::Register(const Ref<Factory> &factory)
	{
		mRegisteredFactories.push_back(factory);
	}

	Ref<Factory> FactoryRegistry::Get(const std::string &extension) const
	{
		for (auto &factory : mRegisteredFactories)
		{
			auto extensions = factory->GetSupportedExtensions();
			auto it = std::find(extensions.begin(), extensions.end(), extension);
			if (it != extensions.end())
				return factory;
		}

		return nullptr;
	}
} // namespace BHive