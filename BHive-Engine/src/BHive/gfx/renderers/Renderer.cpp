#include "gfx/Texture.h"
#include "Renderer.h"
#include "gfx/RenderCommand.h"
#include "gfx/Buffers.h"
#include "gfx/Pipeline.h"

namespace BHive
{
	
	struct BHIVE_API RenderData
	{
		ViewSystem Views;

		Frustum CameraFrustum;

		Ref<Texture> WhiteTexture;
		Ref<Texture> BlackTexture;
		Ref<Texture> BlueTexture;
		Ref<Texture2D> BRDFLut;
		Ref<GeneralBuffer> CameraUBO;

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

			WhiteTexture = Texture2D::Create({1, 1}, create_info, Buffer(& white, sizeof(uint32_t)));

			create_info.DebugName = "Black Texture";
			BlackTexture = Texture2D::Create({1, 1}, create_info, Buffer(&black, sizeof(uint32_t)));

			create_info.DebugName = "Blue Texture";
			BlueTexture = Texture2D::Create({1, 1}, create_info, Buffer(&blue, sizeof(uint32_t)));

			CameraUBO = GeneralBuffer::Create(sizeof(FView), EBufferType::UniformBuffer);	

			BRDFLut = BRDFLUTGenerator::GenerateBRDFLUTMap();
		}
	};

	Renderer::Renderer(Scope<RendererAPI> api)
		: mAPI(std::move(api)),
		  mEnvironment(mGlobalResources)
	{
		ASSERT(mAPI);

		sInstance = this;

		mAPI->Init();

		PipelineRegistry::Initialize();

		mData = CreateRef<RenderData>();

		InitAndRegisterResources();

		Line.Initialize();
		Quad.Initialize();
		Light.Initialize(*this);
	}

	Renderer::~Renderer()
	{
		mAPI->Shutdown();
	}

	void Renderer::BeginFrame()
	{
		mGraph = RenderGraph{};
		mScheduler.BeginFrame(mGraph, mPassConfig);

		mFrameActive = true;
		ResetStats();
		mData->Views.BeginFrame();
		BeginBatching();

		mEnvironment.Update(mScheduler);
	}

	void Renderer::EndFrame()
	{
		mScheduler.Finalize();

		ExecuteGraph(mGraph);

		mFrameActive = false;
	}


	void Renderer::SubmitCamera(const glm::mat4 &projection, const glm::mat4 &view)
	{
		
		FView &v = mData->Views.CreateMainView();

		v.Projection = projection;
		v.View = view;
		v.NearFar.x = projection[3][2] / (projection[2][2] - 1.0f);
		v.NearFar.y = projection[3][2] / (projection[2][2] + 1.0f);
		v.Position = glm::inverse(view)[3];

		mData->CameraFrustum.Update(projection, view);

	}

	void Renderer::Flush()
	{
		EndBatching();
	}

	

	void Renderer::SetEnvironmentTexture(const Ref<Texture2D> &hdr)
	{
		mEnvironment.SetHDR(hdr);
	}

	void Renderer::ExecuteGraph(RenderGraph &graph)
	{
		SolveResourceBarriers(graph);
		mAPI->SubmitGraph(graph);
	}

	FView Renderer::CreateView(const glm::mat4 &projection, const glm::mat4 &view)
	{
		FView v{};

		v.Projection = projection;
		v.View = view;
		v.NearFar.x = projection[3][2] / (projection[2][2] - 1.0f);
		v.NearFar.y = projection[3][2] / (projection[2][2] + 1.0f);
		v.Position = glm::inverse(view)[3];
		return v;
	}

	void Renderer::ResetStats()
	{
		memset(&mStats, 0, sizeof(Statitics));
	}

	GlobalResources &Renderer::GetGlobalResources()
	{
		return mGlobalResources;
	}

	const Frustum &Renderer::GetFrustum()
	{
		ASSERT(mData);
		return mData->CameraFrustum;
	}

	ViewSystem &Renderer::GetViewSystem()
	{
		return mData->Views;
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
		Light.BeginRecording();
	}

	void Renderer::EndBatching()
	{
		Line.Flush(*this);
		Quad.Flush(*this);
		Light.Flush();
	}

	void Renderer::InitAndRegisterResources()
	{
		mGlobalResources.Register("EnvironmentBRDFLUT", mData->BRDFLut);
		mGlobalResources.Register("White", mData->WhiteTexture);
		mGlobalResources.Register("Blue", mData->BlueTexture);
		mGlobalResources.Register("Black", mData->BlackTexture);
		mGlobalResources.Register("Camera", mData->CameraUBO);
	}

	void Renderer::SolveResourceBarriers(RenderGraph &graph)
	{
		std::unordered_map<BufferBase *, EBufferAccess> lastBufferAccess;

		for (auto& pass : graph.GetPasses())
		{
			for (auto& phase : pass.Phases)
			{
				//buffers
				for (auto& use : phase.Buffers)
				{
					auto *raw = use.Buffer;
					auto prev = lastBufferAccess[raw];
					auto next = use.Access;

					if (prev != next)
						phase.CommandList.BufferBarriers.emplace_back(FBufferBarrierRequest{raw, prev, next});

					lastBufferAccess[raw] = next;
				}
			}
		}
	}

} // namespace BHive