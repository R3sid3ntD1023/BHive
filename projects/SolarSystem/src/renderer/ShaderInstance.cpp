#include "ShaderInstance.h"
#include "gfx/UniformBuffer.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"

#define UNIFORM_BUFFER_BINDING 3

namespace BHive
{
	struct PlanetMaterial
	{
		alignas(16) glm::vec3 Color;
		alignas(16) glm::vec3 Emission;
		uint32_t Flags;
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

		mTextures["uTexture"] = {0, nullptr};

		if (!mBuffer)
		{
			mBuffer = CreateRef<UniformBuffer>(UNIFORM_BUFFER_BINDING, sizeof(PlanetMaterial));
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

		for (auto &[name, binding] : mTextures)
		{
			auto texture = binding.second;
			if (texture)
			{
				texture->Bind(binding.first);
			}
		}
	}

	void BHive::ShaderInstance::SetTexture(const char *name, const Ref<Texture> &texture)
	{
		if (!mTextures.contains(name))
		{
			LOG_WARN("No sampler with name {} found!", name);
			return;
		}
		mTextures.at(name).second = texture;
	}
} // namespace BHive