#include "GLBackendMaterial.h"
#include "core/utils/Hash.h"
#include "gfx/BufferBase.h"
#include "gfx/Buffers.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/factories/GFXFactories.h"
#include "gfx/renderers/Renderer.h"

namespace BHive
{

	GLBackendMaterial::GLBackendMaterial(const std::string &shaderProgramName)
		: mShaderProgramName(shaderProgramName)
	{
		Initialize(shaderProgramName);
	}

	void GLBackendMaterial::Initialize(const std::string &shaderProgramName)
	{
		mShaderProgram = ShaderManager::Get(shaderProgramName);
		mShaderTemplate = &mShaderProgram.As<Shader>()->GetTemplate();

		// init set manager

		if (auto set = mShaderTemplate->FindSet(ERenderingSets::MATERIAL))
		{

			for (auto &binding : set->Bindings)
			{
				if (IsBuffer(binding.Type))
				{
					BufferBinding bufferBinding{};
					bufferBinding.Buffer = BufferFactory::Create(binding.Size, BufferTypeFromResource(binding.Type));
					mBufferBindings[binding.Binding] = bufferBinding;
				}

				else if (IsTexture(binding.Type))
				{
					TextureBinding textureBinding{};
					mTextureBindings[binding.Binding] = textureBinding;
				}
			}
		}

		mPushConstantData.resize(mShaderTemplate->TotalPushConstantSize, std::byte(0));
	}

	void GLBackendMaterial::SetTexture(const std::string &name, const TextureBinding &texture)
	{
		auto hash = utils::ComputeHash(name);

		if (auto binding = mShaderTemplate->FindBinding(hash))
		{
			mTextureBindings[binding->Binding] = texture;
			return;
		}
	}

	void GLBackendMaterial::SetParam(const std::string &name, const MaterialParam &param)
	{
		auto hash = utils::ComputeHash(name);

		if (auto u = mShaderTemplate->FindPushConstant(hash))
		{
			memcpy(mPushConstantData.data() + u->Offset, param.Data.data(), param.Size);
			return;
		}

		if (auto binding = mShaderTemplate->FindBinding(hash))
		{
			mBufferBindings[binding->Binding].Buffer.As<BufferBase>()->SetData(param.Data.data(), param.Size);

			return;
		}

		LOG_ERROR("Uniform '{}' not found in shader", name);
	}

	MaterialSnapshot GLBackendMaterial::CreateSnapshot() const
	{
		MaterialSnapshot snapshot{};

		snapshot.Buffers = mBufferBindings;
		snapshot.PushConstantData = mPushConstantData;
		snapshot.Textures = mTextureBindings;
		snapshot.Shader = mShaderProgram;
		snapshot.Flags = mFlags;
		return snapshot;
	}

} // namespace BHive