#pragma once

#include "core/Core.h"
#include "gfx/registries/Handles.h"
#include "core/EnumFlags.h"

namespace BHive
{
	class ResourceSet;

	struct TextureBinding
	{
		Ref<Texture> Texture;
		uint32_t BaseMipLevel = 0;
		uint32_t BaseArrayLayer = 0;
	};

	struct BufferBinding
	{
		BufferPtr Buffer;
	};

	enum class EMaterialFlags : uint8_t
	{
		None = 0,
		DoubleSided = 1 << 0
	};

	ENABLE_BITMASK_OPERATORS(EMaterialFlags)

	struct MaterialSnapshot
	{
		ShaderPtr Shader;

		// binding -> buffer/texture
		std::unordered_map<uint32_t, TextureBinding> Textures;

		// binding -> buffer/texture
		std::unordered_map<uint32_t, BufferBinding> Buffers;

		std::vector<std::byte> PushConstantData;

		EMaterialFlags Flags;
	};
} // namespace BHive