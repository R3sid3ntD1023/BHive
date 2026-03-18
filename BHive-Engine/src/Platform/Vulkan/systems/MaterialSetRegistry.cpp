#include "MaterialSetRegistry.h"
#include "../VulkanSetManager.h"
#include "../VulkanPipeline.h"
#include "../material/VulkanBackendMaterial.h"
#include "../VulkanShader.h"
#include "../VulkanRendererAPI.h"
#include "gfx/RenderCommand.h"
#include "gfx/shader/ShaderReflection.h"
#include "gfx/UniformBuffer.h"
#include "gfx/StorageBuffer.h"
#include "gfx/Texture.h"

namespace BHive
{

	MaterialSetRegistry::Entry &MaterialSetRegistry::CreateForMaterial(VulkanBackendMaterial *mat, const VulkanPipeline *pipeline)
	{
		ASSERT(mat);

		auto program = pipeline->GetShaderProgram();
		auto &merged = program->GetMergedRefl();

		constexpr uint32_t SET = MATERIAL_SET_INDEX;

		if (!merged.Sets.contains(SET))
			return mEntries[mat]; //empty entry

		auto &setRefl = merged.Sets.at(SET);

		auto &shader = pipeline->GetVulkanShader();
		auto &setHashes = shader.GetSetHashes();

		ASSERT(setHashes.contains(SET));
		uint64_t hash = setHashes.at(SET);

		auto &entry = mEntries[mat];
		entry.Owner = mat;
		entry.SetHash = hash;
		entry.SetIndex = SET;

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		entry.Manager = api->CreateSetManager(pipeline, SET);

		BindMaterialResources(mat, setRefl, entry.Manager.get());

		entry.Manager->WriteStaticBindings();

		return entry;
	}

	Ref<ISetManager> MaterialSetRegistry::Find(VulkanBackendMaterial *mat) const
	{
		if (!mEntries.contains(mat))
			return nullptr;

		return mEntries.at(mat).Manager;
	}

	void MaterialSetRegistry::UpdatePerFrame(uint32_t frame)
	{
		for (auto &[mat, entry] : mEntries)
			entry.Manager->Update(frame);
	}

	void MaterialSetRegistry::BindMaterialResources(VulkanBackendMaterial *mat, const FSetReflection &refl, ISetManager *mang)
	{
		for (auto& [name, ub] : refl.UniformBuffers)
		{
			auto ubo = mat->mLocalUBOs.at(name);
			if (ubo)
			{
				mang->SetBuffer(ub.Binding, ubo);
			}
		}

		for (auto &[name, sb] : refl.StorageBuffers)
		{
			auto ssbo = mat->mLocalSSBOs.at(name);
			if (ssbo)
			{
				mang->SetBuffer(sb.Binding, ssbo);
			}
		}

		/*for (auto &[name, sampler] : refl.Samplers)
		{
			auto tex = mat->mTextures.at(name);
			if (tex)
			{
				mang->SetTexture(sampler.Binding, tex);
			}
		}*/
	}
}


