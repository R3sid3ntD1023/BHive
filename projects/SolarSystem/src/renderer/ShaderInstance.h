#pragma once

#include "core/Core.h"
#include "gfx/ShaderDataType.h"

namespace BHive
{
	class Shader;
	class Texture;
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

		void SetTexture(const char *name, const Ref<Texture> &texture);

	private:
		Ref<Shader> mShader;
		uint8_t *mInstanceData = nullptr;
		std::unordered_map<const char *, FShaderUniform> mShaderUniforms;
		std::unordered_map<const char *, std::pair<uint32_t, Ref<Texture>>> mTextures;
		static inline Ref<UniformBuffer> mBuffer;
	};

} // namespace BHive

#include "ShaderInstance.inl"