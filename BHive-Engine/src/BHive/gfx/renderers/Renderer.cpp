#include "gfx/Texture.h"
#include "Renderer.h"
#include "gfx/RenderCommand.h"
#include "gfx/Buffers.h"
#include "gfx/Pipeline.h"

namespace BHive
{

	struct BHIVE_API RenderData
	{
		Ref<Texture> WhiteTexture;
		Ref<Texture> BlackTexture;
		Ref<Texture> BlueTexture;

		RenderData()
		{
			static constexpr uint32_t white = 0xFFFFFFFF;
			static constexpr uint32_t black = 0xFF000000;
			static constexpr uint32_t blue = 0xFF0000FF;

			FTextureCreateInfo create_info{};
			create_info.Format = EFormat::RGBA8;
			create_info.Roles = ETextureRole::Sampled | ETextureRole::TransferDst;
			create_info.Aspect = ETextureAspect::Color;
			create_info.DebugName = "WhiteTexture";

			WhiteTexture = Texture2D::Create({1, 1}, create_info, Buffer(&white, sizeof(uint32_t)));

			create_info.DebugName = "Black Texture";
			BlackTexture = Texture2D::Create({1, 1}, create_info, Buffer(&black, sizeof(uint32_t)));

			create_info.DebugName = "Blue Texture";
			BlueTexture = Texture2D::Create({1, 1}, create_info, Buffer(&blue, sizeof(uint32_t)));
		}
	};

	Renderer::Renderer(Scope<RendererAPI> api)
		: mAPI(std::move(api))
	{
		ASSERT(mAPI);

		sInstance = this;

		mAPI->Init();

		PipelineRegistry::Initialize();

		mData = CreateRef<RenderData>();

		Line.Initialize();
		Quad.Initialize();
	}

	Renderer::~Renderer()
	{
		mData.reset();
		PipelineRegistry::Shutdown();
		mAPI->Shutdown();
	}

	void Renderer::BeginFrame()
	{
		mGraph = RenderGraph{};
		mScheduler.BeginFrame(mGraph, mPassConfig);

		mFrameActive = true;
		ResetStats();
	}

	void Renderer::EndFrame()
	{
		mScheduler.Finalize();

		ExecuteGraph(mGraph);

		mFrameActive = false;
	}

	void Renderer::Flush()
	{
		EndBatching();
	}

	void Renderer::ExecuteGraph(RenderGraph &graph)
	{
		SolveResourceBarriers(graph);
		mAPI->SubmitGraph(graph);
	}

	void Renderer::ResetStats()
	{
		memset(&mStats, 0, sizeof(Statitics));
	}

	Ref<Texture> Renderer::GetWhiteTexture() const
	{
		return mData->WhiteTexture;
	}

	RenderGraph &Renderer::GetActiveGraph()
	{
		if (!mFrameActive)
			BeginFrame();

		return mGraph;
	}

	FPass &Renderer::GetActivePass()
	{
		return mScheduler.GetActivePass();
	}

	FPass &Renderer::BeginPass(const std::string &name, EPassType type, const FPassState &state)
	{
		if (!mFrameActive)
		{
			LOG_WARN("BeginPass called outside a frame. Pass has been deferred.");

			mScheduler.DeferPass(name, type, [=](FPass &pass) {});

			static FPass dummy;
			return dummy;
		}

		return mScheduler.BeginPass(name, type, state);
	}

	void Renderer::DeferPass(const std::string &name, EPassType type, const FPassState &state, std::function<void(FPass &)> fn)
	{
		mScheduler.DeferPass(name, type, fn);
	}

	void Renderer::EndPass()
	{
		mScheduler.EndPass();
	}

	void Renderer::SetPassConfig(const PassConfig &config)
	{
		mPassConfig = config;
	}

	void Renderer::BeginBatching()
	{
		Line.BeginRecording();
		Quad.BeginRecording();
	}

	void Renderer::EndBatching()
	{
		Line.Flush(*this);
		Quad.Flush(*this);
	}

	void Renderer::SolveResourceBarriers(RenderGraph &graph)
	{
		std::unordered_map<Ref<BufferBase>, EBufferUsage> lastBufferAccess;

		for (auto &pass : graph.GetPasses())
		{
			for (auto &phase : pass.Phases)
			{
				// buffers
				for (auto &use : phase.Buffers)
				{
					auto buffer = use.Buffer;
					auto prev = lastBufferAccess[buffer];
					auto next = use.Access;

					if (prev != next)
						phase.CommandList.BufferBarriers.emplace_back(FBufferBarrierRequest{buffer, prev, next});

					lastBufferAccess[buffer] = next;
				}
			}
		}
	}

} // namespace BHive