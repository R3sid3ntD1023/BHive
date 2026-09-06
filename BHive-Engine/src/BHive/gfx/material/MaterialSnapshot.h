#pragma once

#include "core/Core.h"
#include "gfx/registries/Handles.h"

namespace BHive
{
	class ResourceSet;

	struct TextureBinding
	{
		TexturePtr Texture;
		uint32_t BaseMipLevel = 0;
		uint32_t BaseArrayLayer = 0;
	};

	struct BufferBinding
	{
		BufferPtr Buffer;
	};

	struct MaterialSnapshot
	{

		ShaderPtr Shader;

		// binding -> buffer/texture
		std::unordered_map<uint32_t, TextureBinding> Textures;

		// binding -> buffer/texture
		std::unordered_map<uint32_t, BufferBinding> Buffers;

		std::vector<Ref<ResourceSet>> BindingGroups;

		std::vector<std::byte> PushConstantData;
	};
} // namespace BHive