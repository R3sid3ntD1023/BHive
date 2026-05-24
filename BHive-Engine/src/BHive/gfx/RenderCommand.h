#pragma once

#include "RendererAPI.h"

namespace BHive
{
	struct PassConfig
	{
		std::string DefaultPassName = "Default Pass";
		EPassType DefaultPassType = EPassType::SwapChain;
		bool DebugMarkers = false;
	};

	class BHIVE_API RenderCommand
	{
	public:
		
		template <typename TAPI>
			requires(std::is_base_of_v<RendererAPI, TAPI>)
		static TAPI *GetRendererAPI()
		{
			return Cast<TAPI>(sRendererAPI.get());
		}

		static RendererAPI::EAPI GetGraphicsAPI() { return sRendererAPI->GetAPI(); }

		static void Init();

		static void Shutdown();

		static void WaitIdle();

		static void ClearColor(float r, float g, float b, float a = 1.0f);

		static void Clear(ClearMask mask = ClearMask::All);

		static void SetLineWidth(float width);

		static void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

		static void DrawArrays(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count = 0);

		static void DrawElements(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count = 0);

		static void DrawElementsBaseVertex(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t start, uint32_t start_index, uint32_t count = 0, uint32_t instance_count = 1);

		static void DrawElementsRanged(ETopologyMode mode, const Ref<VertexArray>  &vao, uint32_t start, uint32_t end, uint32_t count = 0);

		static void DrawElementsInstanced(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t instances, uint32_t count = 0);

		static void MultiDrawElementsIndirect(ETopologyMode mode, const BufferBase &indirect, const Ref<VertexArray> &vao, size_t drawCount, size_t stride = 0);

		static void Dispatch( const glm::uvec3 &size);
		
		static void AddComputePass(const std::string &name, const std::function<void(FRenderGraphPass&)> &builder);

		static void AddTransferPass(const std::string &name, const std::function<void(FRenderGraphPass &)> &builder);

		static void ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

		static Ref<ISetManager> CreateSetManager(const Pipeline *pipeline, uint32_t setIndex);

		static void SubmitGraph(const RenderGraph &graph, FResourceUpdateList &updateResources);

		static void BeginFrame(); // resets graph

		static RenderGraph &EndFrame(); //returns final graph

		static RenderGraph &GetActiveGraph();

		static FRenderGraphPass &GetActivePass();

		static FRenderGraphPass &BeginPass(const std::string &name, EPassType type);

		static void EndPass();

		static void SubmitResourceUpdate(FResourceUpdateList::UpdateCommand cmd);

		class PassScope
		{
			PassScope(const std::string &name, EPassType type) { RenderCommand::BeginPass(name, type);}

			~PassScope() { RenderCommand::EndPass();}
		};

		static void SetPassConfig(const PassConfig& config);

		static const PassConfig &GetPassConfig() { return sPassConfig; }; 

		static void DebugPass(const std::string &msg);

	private:
		static Scope<RendererAPI> sRendererAPI;
		static inline RenderGraph sGraph{};
		static inline FRenderGraphPass *sActivePass = nullptr;
		static inline bool sFrameActive = false;
		static inline PassConfig sPassConfig{};
	};
} // namespace BHive
