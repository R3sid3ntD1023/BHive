#pragma once

#include "core/Core.h"
#include "PropertyUniform.h"

namespace BHive
{
	class Shader;
	class Texture;
	class UniformBuffer;

	class ShaderInstance
	{
	public:
		ShaderInstance(const Ref<Shader> &shader);
		~ShaderInstance();

		void Bind();

		template <typename T>
		void SetParameter(const std::string &parameterName, const T &v)
		{
			if (mUniforms.contains(parameterName))
			{
				SetData(mUniforms.at(parameterName), &v);
			}
		}

		void SetTexture(const std::string &name, const Ref<Texture> &texture);

	private:
		void SetData(const FUniform &uniform, const void *data);

	private:
		Ref<Shader> mShader;
		std::unordered_map<std::string, FUniform> mUniforms;
		uint8_t *mData = nullptr;
	};

} // namespace BHive