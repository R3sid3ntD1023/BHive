#include "ShaderInstance.h"
#include "gfx/UniformBuffer.h"
#include "gfx/Shader.h"

#define UNIFORM_BUFFER_BINDING 3

namespace BHive
{
	struct PlanetMaterial
	{
		alignas(16) glm::vec3 Color;
		alignas(16) glm::vec3 Emission;
		uint32_t Flags;
		uint64_t Texture;
	};

	ShaderInstance::ShaderInstance(const Ref<Shader> &shader)
		: mShader(shader)
	{
		mInstanceData = (uint8_t *)calloc(1, sizeof(PlanetMaterial));

		mShaderUniforms.reserve(3);
		mShaderUniforms["uColor"] = FShaderUniform{"uColor", offsetof(PlanetMaterial, Color), sizeof(PlanetMaterial::Color)};
		mShaderUniforms["uEmission"] =
			FShaderUniform{"uEmission", offsetof(PlanetMaterial, Emission), sizeof(PlanetMaterial::Emission)};
		mShaderUniforms["uFlags"] = FShaderUniform{"uFlags", offsetof(PlanetMaterial, Flags), sizeof(PlanetMaterial::Flags)};
		mShaderUniforms["uTexture"] =
			FShaderUniform{"uTexture", offsetof(PlanetMaterial, Texture), sizeof(PlanetMaterial::Texture)};

		if (!mBuffer)
		{
			mBuffer = UniformBuffer::Create(UNIFORM_BUFFER_BINDING, sizeof(PlanetMaterial));
		}
	}

	ShaderInstance::~ShaderInstance()
	{
		delete mInstanceData;
	}

	void ShaderInstance::Bind()
	{
		mShader->Bind();

		mBuffer->SetData((const void *)mInstanceData, sizeof(PlanetMaterial));
	}

	void BHive::ShaderInstance::SetTexture(const char *name, uint64_t bindless_texture)
	{
		if (!mShaderSamplers.contains(name))
		{
			LOG_WARN("No sampler with name {} found!", name);
			return;
		}
		mShaderSamplers.at(name) = bindless_texture;
	}
} // namespace BHive