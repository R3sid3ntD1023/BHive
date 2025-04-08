#include "gfx/Shader.h"
#include "ShaderInstance.h"
#include "Texture.h"
#include "UniformBuffer.h"

namespace BHive
{
	ShaderInstance::ShaderInstance(const Ref<Shader> &shader)
		: mShader(shader)
	{
		if (!shader)
			return;

		auto &data = shader->GetRelectionData().at(ShaderType_Fragment).UniformBuffers.at("Material");
		mData = new uint8_t[data.Size];
		// mBuffer = CreateRef<UniformBuffer>(data.Binding, data.Size);
	}

	ShaderInstance::~ShaderInstance()
	{
		delete[] mData;
	}

	void ShaderInstance::Bind()
	{
		mShader->Bind();
	}

	void ShaderInstance::SetTexture(const std::string &name, const Ref<Texture> &texture)
	{
	}

	void ShaderInstance::SetData(const FUniform &uniform, const void *data)
	{
	}

} // namespace BHive