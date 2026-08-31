#include "RegistryManager.h"

#include "gfx/Texture.h"
#include "gfx/sprite/Sprite.h"
#include "gfx/material/Material.h"
#include "gfx/font/Font.h"
#include "gfx/mesh/BaseMesh.h"
#include "gfx/Framebuffer.h"

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
	}

	void ResourceRegistriesManager::Shutdown()
	{
		UnRegisterRegistry<Texture>();
		UnRegisterRegistry<Material>();
		UnRegisterRegistry<Sprite>();
		UnRegisterRegistry<BaseMesh>();
		UnRegisterRegistry<Font>();
		UnRegisterRegistry<Framebuffer>();
	}
} // namespace BHive