#pragma once

#include "core/Core.h"

namespace BHive
{
	class GeneralBuffer;
	class Texture;

	struct MaterialSnapshot
	{
		std::unordered_map<std::string, Ref<GeneralBuffer>> LocalBuffers;

		std::vector<std::byte> PushConstantData;

		struct TextureBinding
		{
			int32_t Binding;
			Ref<Texture> Texture;
			uint32_t Mip;
		};

		std::unordered_map<std::string, TextureBinding> Textures;
	};
} // namespace BHive