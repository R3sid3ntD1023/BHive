#pragma once

#include "core/Core.h"
#include "gfx/ShaderDataType.h"

namespace BHive
{
	class Shader;
	class UniformBuffer;

	struct FShaderUniform
	{
		const char *Name;
		uint32_t Offset;
		size_t Size;
		EShaderDataType Type;
	};

	class ShaderInstance
	{
	public:
		ShaderInstance(const Ref<Shader> &shader);
		~ShaderInstance();

		void Bind();

		template <typename T>
		void SetParameter(const char *parameterName, const T &v);

		void SetTexture(const char *name, uint64_t bindless_texture);

	private:
		Ref<Shader> mShader;
		uint8_t *mInstanceData = nullptr;
		std::unordered_map<const char *, FShaderUniform> mShaderUniforms;
		std::unordered_map<const char *, uint64_t> mShaderSamplers;
		static inline Ref<UniformBuffer> mBuffer;
	};

} // namespace BHive

#include "ShaderInstance.inl"