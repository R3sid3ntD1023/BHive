#include "RegistryManager.h"
#include "ResourceRegistry.h"
#include "gfx/Framebuffer.h"
#include "gfx/Texture.h"
#include "gfx/font/Font.h"
#include "gfx/material/Material.h"
#include "gfx/mesh/BaseMesh.h"
#include "gfx/sprite/Sprite.h"

namespace BHive
{
	void ResourceRegistriesManager::Init()
	{
		RegisterRegistry<Texture>();
		RegisterRegistry<Material>();
		RegisterRegistry<Sprite>();
		RegisterRegistry<BaseMesh>();
		RegisterRegistry<Font>();
		RegisterRegistry<Framebuffer>();
		RegisterRegistry<BufferBase>();
		RegisterRegistry<VertexArray>();
	}

	void ResourceRegistriesManager::Shutdown()
	{
		Registries.clear();

		UnRegisterRegistry<Texture>();
		UnRegisterRegistry<Material>();
		UnRegisterRegistry<Sprite>();
		UnRegisterRegistry<BaseMesh>();
		UnRegisterRegistry<Font>();
		UnRegisterRegistry<Framebuffer>();
		UnRegisterRegistry<BufferBase>();
		UnRegisterRegistry<VertexArray>();
	}

	IResourceRegistry *ResourceRegistriesManager::GetRegistry(uint32_t type)
	{
		auto it = Registries.find(type);
		if (it == Registries.end())
		{
			ASSERT(false, "Registry not found for type");
			return nullptr;
		}

		return it->second;
	}
} // namespace BHive