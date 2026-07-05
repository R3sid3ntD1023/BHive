#pragma once

#include "core/Core.h"
#include "LineRenderer.h"
#include "QuadRenderer.h"
#include "gfx/RendererAPI.h"
#include "gfx/GlobalResources.h"
#include "PMREMGenerator.h"
#include "EnvironmentSystem.h"
#include "RenderGraphScheduler.h"
#include "ViewSystem.h"
#include "Lights.h"

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
		LineRenderer Line;
		QuadRenderer Quad;
		Lights Light;

	public:
		Renderer(Scope<RendererAPI> api);

		~Renderer();

		struct BHIVE_API Statitics
		{
			uint32_t DrawCalls;
			uint32_t InstanceCount;
		};

		void BeginFrame();

		void SubmitCamera(const glm::mat4 &projection, const glm::mat4 &view);

		void Flush();

		void EndFrame();

		void SetEnvironmentTexture(const Ref<Texture2D> &hdr);

		void ExecuteGraph(RenderGraph &graph);

		FView CreateView(const glm::mat4 &projection, const glm::mat4 &view);

		const Frustum &GetFrustum();

		void ResetStats();

		const Statitics &GetStats() const { return mStats; }

		GlobalResources &GetGlobalResources();

		static Renderer& Get() { return *sInstance;}


#pragma region RENDERGRAPH

		ViewSystem &GetViewSystem();

		RenderGraph &GetActiveGraph();

		FPass &GetActivePass();

		FPass &BeginPass(const std::string &name, EPassType type, const FPassState& state = {});

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
		void BeginBatching();

		void EndBatching();

		void InitAndRegisterResources();

		void SolveResourceBarriers(RenderGraph &graph);

	private:
		Scope<RendererAPI> mAPI;

		Ref<struct RenderData> mData;

		GlobalResources mGlobalResources;

		EnvironmentSystem mEnvironment;

		RenderGraphScheduler mScheduler;
		
		//rendergraph
		RenderGraph mGraph;

		PassConfig mPassConfig;

		bool mFrameActive = false;

		Statitics mStats{};

		static inline Renderer *sInstance = nullptr;
	};

} // namespace BHive
