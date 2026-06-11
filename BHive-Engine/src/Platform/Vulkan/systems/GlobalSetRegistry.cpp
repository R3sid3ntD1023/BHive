#include "GlobalSetRegistry.h"
#include "gfx/shader/ShaderReflection.h"
#include "../VulkanPipeline.h"
#include "../VulkanShader.h"
#include "gfx/RenderCommand.h"
#include "gfx/renderers/Renderer.h"
#include "SetSemantics.h"

namespace BHive
{

	GlobalSetEntry &GlobalSetRegistry::EnsureGlobalSet(const VulkanPipeline &pipeline, uint32_t setIndex)
	{
		auto setSemantic = GetSetSemantic(setIndex);
		ASSERT(setSemantic == ESetSemantic::Global, "GlobalSetRegistry should only be for global sets");

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

		auto api = RenderCommand::GetGraphicsAPI();
		entry.Manager = api->CreateSetManager(&pipeline, setIndex);

		auto& shaderName = pipeline.GetShaderProgram()->GetName();
		BindGlobalResources(refl, entry.Manager.get(), setIndex, shaderName);

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

	void GlobalSetRegistry::BindGlobalResources(const FShaderReflectionLookUp &refl, ISetManager *mang, uint32_t setIndex, const std::string& shaderName)
	{
		auto &globals = Renderer::Get().GetGlobalResources();
		auto &bindings = refl.GetSetBindings(setIndex);

		if (bindings.empty())
		{
			LOG_INFO("GlobalSetRegistry: Shader '{}' has no global resources in set {}, skipping global binding.", shaderName, setIndex);
			return;
		}

		for (auto &r : bindings)
		{
			const std::string semantic =  r.Semantic.empty() ? r.name : r.Semantic;

			if (r.Semantic.empty())
			{
				LOG_ERROR(
					"Shader '{}' has resource '{}' in set {} binding {} with NO semantic tag.\n"
					"Add: // @semantic <Name> above the declaration.",
					shaderName, r.name, setIndex, r.binding);
			}

			auto *res = globals.Find(semantic);

			if (!res)
			{
				auto guess = globals.GuessSemanticFromName(semantic);
				if (!guess.empty())
				{
					LOG_WARN(
						"Shader '{}' variable '{}' requested semantic '{}', but it was not found.\n"
						"   Did you mean semantic '{}'?",
						shaderName, r.name, semantic, guess);
				}

				LOG_ERROR(
					"GlobalSet Binding Error:\n"
					"  Shader: {}\n"
					"  Set: {}\n"
					"  Binding: {}\n"
					"  Shader Variable: '{}'\n"
					"  Semantic Requested: '{}'\n"
					"  BUT GlobalResources does not contain this semantic.\n"
					"  Registered Global Semantics: {}",
					shaderName, setIndex, r.binding, r.name, semantic, globals.DebugListSemantics());

				ASSERT(false, "Missing global semantic")

				continue;
			}
			else if (res->IsBuffer() && IsBuffer(r.kind))
			{
				mang->SetBuffer(r.binding, res->BufferRef);
				LOG_INFO("GlobalSet: bound BUFFER '{}' (semantic '{}') at set {}, binding {}", r.name, semantic, setIndex, r.binding);
				continue;
			}
			
			if (IsTexture(r.kind))
			{
				mang->SetTexture(r.binding, res->TextureRef);
				LOG_INFO("GlobalSet: bound TEXTURE '{}' (semantic '{}') at set {}, binding {}", r.name, semantic, setIndex, r.binding);
				continue;
			}

			
		}
	}
}

