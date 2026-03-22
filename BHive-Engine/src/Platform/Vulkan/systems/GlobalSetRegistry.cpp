#include "GlobalSetRegistry.h"
#include "gfx/shader/ShaderReflection.h"
#include "../VulkanPipeline.h"
#include "../VulkanShader.h"
#include "../VulkanRendererAPI.h"
#include "gfx/RenderCommand.h"
#include "gfx/GlobalBuffers.h"


namespace BHive
{

	GlobalSetEntry &GlobalSetRegistry::EnsureGlobalSet(const VulkanPipeline &pipeline, uint32_t setIndex)
	{
		auto program = pipeline.GetShaderProgram();
		auto &refl = program->GetRefl();

		auto &shader = pipeline.GetVulkanShader();
		auto &setHashes = shader.GetSetHashes();

		ASSERT(setHashes.contains(setIndex));
		uint64_t hash = setHashes.at(setIndex);

		// already created?
		if (mEntries.contains(hash))
			return mEntries.at(hash);

		// create new entry
		auto &entry = mEntries[hash];
		entry.Key = {hash, setIndex};

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		entry.Manager = api->CreateSetManager(&pipeline, setIndex);

		BindGlobalResources(refl, entry.Manager.get(), setIndex);

		entry.Manager->WriteStaticBindings();

		return entry;
	}

	Ref<ISetManager> GlobalSetRegistry::Find(uint64_t setHash) const
	{
		if (!mEntries.contains(setHash))
			return nullptr;

		return mEntries.at(setHash).Manager;
	}

	void GlobalSetRegistry::UpdatePerFrame(uint32_t frame)
	{
		for (auto &[_, entry] : mEntries)
			entry.Manager->Update(frame);
	}

	void GlobalSetRegistry::BindGlobalResources(const FShaderReflectionLookUp &refl, ISetManager *mang, uint32_t setIndex)
	{
		auto &globals = GetSubSystem<GlobalBuffers>();
		auto &bindings = refl.GetSetBindings(setIndex);

		for (auto &r : bindings)
		{
			if (IsBuffer(r.kind))
			{
				if (globals.GetBuffers().contains(r.binding))
				{
					mang->SetBuffer(r.binding, globals.GetBuffers().at(r.binding));
					LOG_INFO("Binding global buffer {} at binding {}: {}", r.name, r.binding, globals.GetBuffers().contains(r.binding));
				}
				
			}
			else if (IsTexture(r.kind))
			{
				if (globals.GetTextures().contains(r.binding))
					mang->SetTexture(r.binding, globals.GetTextures().at(r.binding));
			}
		}

		mang->WriteStaticBindings();
	}
}

