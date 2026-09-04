#include "VulkanBackendMaterial.h"
#include "VulkanBindingGroup.h"
#include "VulkanShader.h"
#include "gfx/BufferBase.h"
#include "gfx/Buffers.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/factories/GFXFactories.h"

namespace BHive
{

	VulkanBackendMaterial::VulkanBackendMaterial(const std::string &shaderProgramName)
	{
		mShaderProgram = ShaderManager::Get(shaderProgramName);
		auto &mergedRefl = mShaderProgram.As<VulkanShader>()->GetMergedRefl();

		// init set manager

		if (mergedRefl.Sets.contains(MATERIAL_SET_INDEX))
		{
			auto set = mergedRefl.Sets.at(MATERIAL_SET_INDEX);
			CreateLocalBuffers(set);
		}

		CreatePushConstanstData(mergedRefl.PushConstants);

		auto vkShader = mShaderProgram.As<VulkanShader>();
		for (auto &set : mergedRefl.Sets)
		{
			mBindGroups.emplace_back(CreateRef<VulkanBindingGroup>(vkShader, set.first));
		}
	}

	void VulkanBackendMaterial::SetTexture(const std::string &name, const FTextureBinding &texture)
	{
		auto &mergedRefl = mShaderProgram.As<VulkanShader>()->GetMergedRefl();
		if (auto sampler = mergedRefl.FindSampler(name, MATERIAL_SET_INDEX))
		{
			MaterialSnapshot::TextureBinding binding{};
			binding.Texture = texture.Texture;
			binding.BaseMipLevel = texture.BaseMipLevel;
			binding.BaseArrayLayer = texture.BaseArrayLayer;
			binding.Binding = sampler->Binding;
			mTextureBindings[name] = binding;
		}
	}

	void VulkanBackendMaterial::SetParam(const std::string &name, const MaterialParam &param)
	{
		auto shader = mShaderProgram.As<VulkanShader>();
		auto &mergedRefl = shader->GetMergedRefl();

		if (auto u = mergedRefl.FindPushConstant(name))
		{
			memcpy(mPushConstantData.data() + u->Offset, param.Data.data(), param.Size);
			return;
		}

		if (mLocalBuffers.contains(name))
		{
			mLocalBuffers[name].Buffer.As<BufferBase>()->SetData(param.Data.data(), param.Size);
			return;
		}

		LOG_ERROR("Uniform '{}' not found in shader '{}'", name, shader->GetName());
	}

	MaterialSnapshot VulkanBackendMaterial::CreateSnapshot() const
	{
		auto shader = mShaderProgram.As<VulkanShader>();

		MaterialSnapshot snapshot{};

		snapshot.LocalBuffers = mLocalBuffers;
		snapshot.PushConstantData = mPushConstantData;
		snapshot.Textures = mTextureBindings;
		snapshot.Shader = mShaderProgram;
		snapshot.mReflection = &shader->GetMergedRefl();
		snapshot.ReflectionLookUp = &shader->GetRefl();
		snapshot.BindingGroups = mBindGroups;

		return snapshot;
	}

	void VulkanBackendMaterial::CreateLocalBuffers(const FSetReflection &set)
	{
		// create local buffers
		for (auto &[name, ubo] : set.UniformBuffers)
		{
			MaterialSnapshot::BufferBinding binding{};
			binding.Buffer = BufferFactory::Create(ubo.Size, EBufferType::UniformBuffer);
			binding.Binding = ubo.Binding;
			mLocalBuffers.emplace(name, binding);
		}

		for (auto &[name, ssbo] : set.StorageBuffers)
		{
			MaterialSnapshot::BufferBinding binding{};
			binding.Buffer = BufferFactory::Create(ssbo.Size, EBufferType::StorageBuffer);
			binding.Binding = ssbo.Binding;
			mLocalBuffers.emplace(name, binding);
		}
	}

	void VulkanBackendMaterial::CreatePushConstanstData(const std::vector<FPushConstantsRange> &ranges)
	{
		// create push constant buffer
		size_t total_size = 0;
		for (auto &pc : ranges)
			total_size = std::max(total_size, (size_t)pc.Offset + pc.Size);

		mPushConstantData.resize(total_size);
	}
} // namespace BHive