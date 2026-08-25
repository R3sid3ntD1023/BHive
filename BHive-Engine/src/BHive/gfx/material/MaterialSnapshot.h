#pragma once

#include "core/Core.h"

namespace BHive
{
	class GeneralBuffer;
	class Texture;
	class ShaderProgram;
	class IBindingGroup;

	struct MaterialSnapshot
	{
		struct TextureBinding
		{
			Ref<Texture> TextureRef;
			int32_t Binding = 0;
			uint32_t BaseMipLevel = 0;
			uint32_t BaseArrayLayer = 0;
		};

		struct BufferBinding
		{
			Ref<GeneralBuffer> BufferRef;
			int32_t Binding = 0;
		};

		Ref<ShaderProgram> Shader;

		std::unordered_map<std::string, TextureBinding> Textures;

		std::unordered_map<std::string, BufferBinding> LocalBuffers;

		std::vector<Ref<IBindingGroup>> BindingGroups;

		std::vector<std::byte> PushConstantData;

		const struct FShaderReflectionLookUp *ReflectionLookUp = nullptr;

		const struct FShaderReflection *mReflection = nullptr;
	};
} // namespace BHive