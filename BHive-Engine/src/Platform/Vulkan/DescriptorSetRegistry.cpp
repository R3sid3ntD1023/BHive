#include "DescriptorSetRegistry.h"
#include "VulkanBackend.h"
#include "VulkanPipeline.h"
#include "material/VulkanBackendMaterial.h"
#include "VulkanShader.h"
#include "DescriptorSetManager.h"
#include "gfx/GlobalResources.h"
#include "VulkanRendererAPI.h"
#include "gfx/RenderCommand.h"
#include "gfx/Buffers.h"

namespace BHive
{

	Ref<DescriptorSetManager> DescriptorSetRegistry::GetSet(const VulkanPipeline &pipeline, uint32_t setIndex, VulkanBackendMaterial *backendMat, uint32_t frame)
	{
		if (setIndex == GLOBAL_SET_INDEX)
			return GetGlobalSet(pipeline, frame);

		if (setIndex == MATERIAL_SET_INDEX)
			return GetMaterialSet(pipeline, backendMat, frame);

		return GetObjectSet(pipeline, setIndex, frame);
	}

	Ref<DescriptorSetManager> DescriptorSetRegistry::GetGlobalSet(const VulkanPipeline &pipeline, uint32_t frame)
	{
		const auto &shader = pipeline.GetVulkanShader();
		const auto &setHashes = shader.GetSetHashes();

		if (!setHashes.contains(GLOBAL_SET_INDEX))
			return nullptr;

		uint64_t hash = setHashes.at(GLOBAL_SET_INDEX);
		GlobalKey key{&pipeline};
		if (!mGlobalSets.contains(key))
		{
			auto prog = pipeline.GetShaderProgram();
			auto &refl = prog->GetRefl();
			auto layout = pipeline.GetSetLayout(GLOBAL_SET_INDEX);

			auto api = RenderCommand::GetGraphicsAPI<VulkanRendererAPI>();
			vk::DescriptorPool pool = api->GetDescriptorPool();

			DescriptorSetProvider<GlobalKey> provider{};
			provider.Key = key;
			provider.Manager = CreateRef<DescriptorSetManager>(VulkanBackend::GetLogicalDevice(), pool, layout, GLOBAL_SET_INDEX, refl);

			auto &merged = prog->GetMergedRefl();
			auto &setRefl = merged.Sets.at(GLOBAL_SET_INDEX);

			for (auto& [name, ub] : setRefl.UniformBuffers)
			{
				auto& ubo = GlobalResources::Get().GetBuffer(ub.Binding);
				if (!ubo)
					continue;
				provider.Manager->Write({uint32_t(ub.Binding), ubo});
			}

			for (auto &[name, sb] : setRefl.StorageBuffers)
			{
				auto &ssbo = GlobalResources::Get().GetBuffer(sb.Binding);
				if (!ssbo)
					continue;
				provider.Manager->Write({uint32_t(sb.Binding), ssbo});
			}

			for (auto &[name, smp] : setRefl.Samplers)
			{
				auto &tex = GlobalResources::Get().GetTexture(smp.Binding);
				if (!tex)
					continue;
				provider.Manager->Write({uint32_t(smp.Binding), 0, tex});
			}

			mGlobalSets.emplace(key, provider);
		}

		auto& entry = mGlobalSets.at(key);
		entry.Manager->Update(frame);
		return entry.Manager;
	}

	Ref<DescriptorSetManager> DescriptorSetRegistry::GetMaterialSet(const VulkanPipeline &pipeline, VulkanBackendMaterial *mat, uint32_t frame)
	{
		if (!mat)
			return nullptr;

		MaterialKey key{mat, &pipeline};
		if (!mMaterialSets.contains(key))
		{
			auto prog = pipeline.GetShaderProgram();
			auto &refl = prog->GetRefl();
			auto &merged = prog->GetMergedRefl();

			if (!merged.Sets.contains(MATERIAL_SET_INDEX))
				return nullptr;

			auto &setRefl = merged.Sets.at(MATERIAL_SET_INDEX);
			auto layout = pipeline.GetSetLayout(MATERIAL_SET_INDEX);

			auto api = RenderCommand::GetGraphicsAPI<VulkanRendererAPI>();
			vk::DescriptorPool pool = api->GetDescriptorPool();

			DescriptorSetProvider<MaterialKey> provider{};
			provider.Key = key;
			provider.Manager = CreateRef<DescriptorSetManager>(VulkanBackend::GetLogicalDevice(), pool, layout, MATERIAL_SET_INDEX, refl);

			for (auto& [name, ub] : setRefl.UniformBuffers)
			{
				auto& ubo = mat->mLocalBuffers.at(name);
				if (!ubo)
					continue;
				provider.Manager->Write({uint32_t(ub.Binding), ubo});
			}

			for (auto &[name, ub] : setRefl.StorageBuffers)
			{
				auto &ssbo = mat->mLocalBuffers.at(name);
				if (!ssbo)
					continue;
				provider.Manager->Write({uint32_t(ub.Binding), ssbo});
			}

			mMaterialSets.emplace(key, provider);
		}

		
		auto &entry = mMaterialSets.at(key);
		entry.Manager->Update(frame);
		return entry.Manager;
	}

	Ref<DescriptorSetManager> DescriptorSetRegistry::GetObjectSet(const VulkanPipeline &pipeline, uint32_t setIndex, uint32_t frame)
	{
		ObjectKey key{&pipeline, setIndex};
		if (!mObjectSets.contains(key))
		{
			auto prog = pipeline.GetShaderProgram();
			auto &refl = prog->GetRefl();
			auto layout = pipeline.GetSetLayout(MATERIAL_SET_INDEX);

			auto api = RenderCommand::GetGraphicsAPI<VulkanRendererAPI>();
			vk::DescriptorPool pool = api->GetDescriptorPool();

			DescriptorSetProvider<ObjectKey> provider{};
			provider.Key = key;
			provider.Manager = CreateRef<DescriptorSetManager>(VulkanBackend::GetLogicalDevice(), pool, layout, MATERIAL_SET_INDEX, refl);

			mObjectSets.emplace(key, provider);
		}

		
		auto &entry = mObjectSets.at(key);
		entry.Manager->Update(frame);
		return entry.Manager;
	}
} // namespace BHive