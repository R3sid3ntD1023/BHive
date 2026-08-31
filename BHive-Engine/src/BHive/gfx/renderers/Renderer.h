#pragma once

#include "core/Core.h"
#include "LineRenderer.h"
#include "QuadRenderer.h"
#include "gfx/RendererAPI.h"
#include "RenderGraphScheduler.h"
#include "ViewSystem.h"
#include "gfx/ShaderManager.h"
#include "gfx/registries/IResourceRegistry.h"

namespace BHive
{

	class Texture;
	class VertexArray;
	class BufferBase;

	struct FPendingPass
	{
		std::string Name;
		EPassType Type;
		std::function<void(FPass &)> BuildFunc;
	};

	class BHIVE_API Renderer
	{

	public:
		Renderer(Scope<RendererAPI> api);

		~Renderer();

		struct BHIVE_API Statitics
		{
			uint32_t DrawCalls;
			uint32_t InstanceCount;
		};

		void BeginFrame();

		void Flush();

		void EndFrame();

		void ExecuteGraph(RenderGraph &graph);

		void ResetStats();

		const Statitics &GetStats() const { return mStats; }

		Texture2DPtr GetWhiteTexture() const;

		void BeginBatching();

		void EndBatching();

		ShaderManager &GetShaderManager() { return mShaderManager; }

		static Renderer &Get() { return *sInstance; }

#pragma region RENDERGRAPH

		RenderGraph &GetActiveGraph();

		FPass &GetActivePass();

		FPass &BeginPass(const std::string &name, EPassType type, const FPassState &state = {});

		void DeferPass(const std::string &name, EPassType type, const FPassState &state = {}, std::function<void(FPass &)> fn = {});

		void EndPass();

		void SetPassConfig(const PassConfig &config);

		const PassConfig &GetPassConfig() { return mPassConfig; };

#pragma endregion

		template <typename T>
			requires(std::is_base_of_v<RendererAPI, T>)
		inline T *GetGraphicsAPI() const
		{
			return Cast<T>(mAPI.get());
		}

		inline RendererAPI *GetGraphicsAPI() const { return mAPI.get(); }

	private:
		Scope<RendererAPI> mAPI;

		ShaderManager mShaderManager;

		Ref<struct RenderData> mData;

		RenderGraphScheduler mScheduler;

		// rendergraph
		RenderGraph mGraph;

		PassConfig mPassConfig;

		bool mFrameActive = false;

		Statitics mStats{};

		static inline Renderer *sInstance = nullptr;

	public:
		LineRenderer Line;

		QuadRenderer Quad;
	};

} // namespace BHive
