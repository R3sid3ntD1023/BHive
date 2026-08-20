#pragma once

#include "core/Core.h"
#include "LineRenderer.h"
#include "QuadRenderer.h"
#include "gfx/RendererAPI.h"
#include "RenderGraphScheduler.h"
#include "ViewSystem.h"

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

	struct FPerObjectData
	{
		glm::mat4 WorldMatrix = glm::identity<glm::mat4>();
	};

	class BHIVE_API Renderer
	{
	public:
		LineRenderer Line;
		QuadRenderer Quad;

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

		Ref<Texture> GetWhiteTexture() const;

		void BeginBatching();

		void EndBatching();

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
		void SolveResourceBarriers(RenderGraph &graph);

	private:
		Scope<RendererAPI> mAPI;

		Ref<struct RenderData> mData;

		RenderGraphScheduler mScheduler;

		// rendergraph
		RenderGraph mGraph;

		PassConfig mPassConfig;

		bool mFrameActive = false;

		Statitics mStats{};

		static inline Renderer *sInstance = nullptr;
	};

} // namespace BHive
