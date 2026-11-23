#pragma once

#include "gfx/material/BackendMaterial.h"

namespace BHive
{
	struct ShaderTemplate;

	class GLBackendMaterial : public IMaterialBackendInterface
	{
	public:
		explicit GLBackendMaterial(const std::string &shaderProgramName);

		void SetTexture(const std::string &name, const TextureBinding &texture) override;

		void SetParam(const std::string &name, const MaterialParam &param) override;

		MaterialSnapshot CreateSnapshot() const;

	private:
		void Initialize(const std::string &shaderProgramName);

	private:
		ShaderPtr mShaderProgram;

		std::string mShaderProgramName = "";

		const ShaderTemplate *mShaderTemplate = nullptr;

		std::vector<std::byte> mPushConstantData;

		// binding -> buffer/texture
		std::unordered_map<uint32_t, BufferBinding> mBufferBindings;

		std::unordered_map<uint32_t, TextureBinding> mTextureBindings;
	};
} // namespace BHive