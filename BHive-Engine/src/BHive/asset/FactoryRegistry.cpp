#include "FactoryRegistry.h"
#include "factories/AudioFactory.h"
#include "factories/SpriteFactory.h"
#include "factories/TextureFactory.h"

namespace BHive
{
	FactoryRegistry::FactoryRegistry()
	{
		Register(Register<AudioFactory>(), {".ogg", ".wav"});
		Register(Register<TextureFactory>(), {".png", ".jpg", ".jpeg"});
		Register(Register<SpriteFactory>(), {".sprite"});
	}

	void FactoryRegistry::Register(Factory *factory, std::initializer_list<std::string> exts)
	{
		for (auto &ext : exts)
		{
			mExtensionMap.emplace(ext, factory);
		}
	}

	Factory *FactoryRegistry::Find(const rttr::type &type) const
	{
		if (mTypeMap.contains(type.get_id()))
		{
			return mTypeMap.at(type.get_id());
		}

		return nullptr;
	}

	Factory *FactoryRegistry::Find(std::string_view ext) const
	{
		if (mExtensionMap.contains(ext.data()))
		{
			return mExtensionMap.at(ext.data());
		}

		return nullptr;
	}
} // namespace BHive