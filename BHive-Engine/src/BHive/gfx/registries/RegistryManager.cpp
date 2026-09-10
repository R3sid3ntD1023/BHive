#include "RegistryManager.h"
#include "ResourceRegistry.h"
#include "gfx/Framebuffer.h"
#include "gfx/Pipeline.h"
#include "gfx/ResourceSet.h"
#include "gfx/Texture.h"
#include "gfx/animation/SkeletalAnimation.h"
#include "gfx/animation/Skeleton.h"
#include "gfx/font/Font.h"
#include "gfx/material/Material.h"
#include "gfx/mesh/BaseMesh.h"
#include "gfx/shader/Shader.h"
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
		RegisterRegistry<Pipeline>();
		RegisterRegistry<Shader>();
		RegisterRegistry<ResourceSet>();
		RegisterRegistry<Skeleton>();
		RegisterRegistry<SkeletalAnimation>();
	}

	void ResourceRegistriesManager::Shutdown()
	{

		UnRegisterRegistry<Texture>();
		UnRegisterRegistry<Material>();
		UnRegisterRegistry<Sprite>();
		UnRegisterRegistry<BaseMesh>();
		UnRegisterRegistry<Font>();
		UnRegisterRegistry<Framebuffer>();
		UnRegisterRegistry<BufferBase>();
		UnRegisterRegistry<VertexArray>();
		UnRegisterRegistry<Pipeline>();
		UnRegisterRegistry<Shader>();
		UnRegisterRegistry<ResourceSet>();
		UnRegisterRegistry<Skeleton>();
		UnRegisterRegistry<SkeletalAnimation>();

		Registries.clear();
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