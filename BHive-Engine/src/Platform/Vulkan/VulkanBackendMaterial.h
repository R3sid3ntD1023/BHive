#pragma once

#include "VulkanCore.h"
#include "gfx/material/BackendMaterial.h"
#include "gfx/shader/ShaderReflection.h"

namespace BHive
{
	class ShaderProgram;
	class VulkanShader;
	struct FShaderReflection;
	class GeneralBuffer;
	class VulkanPipeline;

	class VulkanBackendMaterial : public IMaterialBackendInterface
	{
	public:
		explicit VulkanBackendMaterial(const std::string &shaderProgramName);

		void SetTexture(const std::string &name, const FTextureBinding &texture) override;

		void SetParam(const std::string &name, const MaterialParam &param) override;

		MaterialSnapshot CreateSnapshot() const;

	private:
		void CreateLocalBuffers(const FSetReflection &set);

		void CreatePushConstanstData(const std::vector<FPushConstantsRange> &ranges);

	private:
		Ref<ShaderProgram> mProgram;

		std::vector<std::byte> mPushConstantData;

		std::unordered_map<std::string, MaterialSnapshot::BufferBinding> mLocalBuffers;

		std::unordered_map<std::string, MaterialSnapshot::TextureBinding> mTextureBindings;

		std::vector<Ref<IBindingGroup>> mBindGroups;
	};
} // namespace BHive