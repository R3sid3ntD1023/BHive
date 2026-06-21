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

		Ref<GPUBuffer> CameraUBO;

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

			CameraUBO = GPUBuffer::Create(sizeof(FView), EBufferType::UniformBuffer);		
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

		auto brdfLUT = mPMREMGenerator.GenerateBRDFLUTMap();
		mGlobalResources.Register("EnvironmentBRDFLUT", brdfLUT);
		mGlobalResources.Register("White", mData->WhiteTexture);
		mGlobalResources.Register("Blue", mData->BlueTexture);
		mGlobalResources.Register("Black", mData->BlackTexture);
		mGlobalResources.Register("Camera", mData->CameraUBO);

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
		ResetStats();

		mData->Views.BeginFrame();

		Line.BeginRecording();
		Quad.BeginRecording();
		Light.BeginRecording();

		mGraph = RenderGraph{};
		mActivePass = nullptr;
		mFrameActive = true;

		mResourceUpdates.Clear();
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
		Line.Flush(*this);
		Quad.Flush(*this);
		Light.Flush();
	}

	void Renderer::EndFrame()
	{
		
		RenderCommand::Flush(mAPI.get());

		mAPI->SubmitGraph(mGraph, mResourceUpdates);

		mResourceUpdates.Clear();

		mFrameActive = false;
	}


	void Renderer::SetEnvironmentTexture(const Ref<Texture2D> &texture)
	{
		auto cube_map = mPMREMGenerator.GenerateEnvironmentCubeMap(texture);
		auto irradiance = mPMREMGenerator.GenerateIrradianceMap(cube_map);
		auto prefilter = mPMREMGenerator.GeneratePreFilteredEnvironmentMap(irradiance);

		mGlobalResources.Register("EnvironmentCubeMap", cube_map);
		mGlobalResources.Register("EnvironmentIrradiance", irradiance);
		mGlobalResources.Register("EnvironmentPreFilter", prefilter);
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

	void Renderer::ClearColor(float r, float g, float b, float a)
	{
		auto pass = &GetActivePass();
		mAPI->ClearColor(pass, r, g, b, a);
	}

	void Renderer::Clear(ClearMask mask)
	{
		auto pass = &GetActivePass();
		mAPI->Clear(pass, mask);
	}

	void Renderer::SetLineWidth(float width)
	{
		auto pass = &GetActivePass();
		mAPI->SetLineWidth(pass, width);
	}

	void Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		auto pass = &GetActivePass();
		mAPI->SetViewport(pass, x, y, w, h);
	}

	void Renderer::DrawArrays(ETopologyMode mode, VertexArray* vao, uint32_t count)
	{
		auto pass = &GetActivePass();
		mAPI->DrawArrays(pass, mode, vao, count);
	}

	void Renderer::DrawElements(ETopologyMode mode, VertexArray* vao, uint32_t count)
	{
		auto pass = &GetActivePass();
		mAPI->DrawElements(pass, mode, vao, count);
	}

	void Renderer::DrawElementsBaseVertex(ETopologyMode mode, VertexArray* vao, uint32_t start, uint32_t start_index, uint32_t count, uint32_t instance_count)
	{
		auto pass = &GetActivePass();
		mAPI->DrawElementsBaseVertex(pass, mode, vao, start, start_index, count, instance_count);
	}

	void Renderer::DrawElementsRanged(ETopologyMode mode, VertexArray* vao, uint32_t start, uint32_t end, uint32_t count)
	{
		auto pass = &GetActivePass();
		mAPI->DrawElementsRanged(pass, mode, vao, start, end, count);
	}

	void Renderer::DrawElementsInstanced(ETopologyMode mode, VertexArray* vao, uint32_t instances, uint32_t count)
	{
		auto pass = &GetActivePass();
		mAPI->DrawElementsInstanced(pass, mode, vao, instances, count);
	}

	void Renderer::MultiDrawElementsIndirect(ETopologyMode mode, BufferBase* indirect, VertexArray* vao, uint32_t drawCount, uint32_t stride, uint32_t start)
	{
		auto pass = &GetActivePass();
		mAPI->MultiDrawElementsIndirect(pass, mode, indirect, vao, drawCount, stride, stride * start);
	}

	FAsyncPass *Renderer::ExecuteComputePass(Pipeline *pipeline, const glm::uvec3 &dispatchSize, const FComputeFunc &builder)
	{
		return mAPI->ExecuteComputePass(pipeline, dispatchSize, builder);
	}

	void Renderer::ExecuteTransferPass(FTransferFunc &&builder)
	{
		mAPI->ExecuteTransferPass(std::move(builder));
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

	FRenderGraphPass &Renderer::GetActivePass()
	{
		if (!mActivePass)
		{
			auto &graph = GetActiveGraph();
			mActivePass = &graph.AddPass(mPassConfig.DefaultPassName, mPassConfig.DefaultPassType);

			if (mPassConfig.DebugMarkers)
				DebugPass("AutoDefaultPass: " + mPassConfig.DefaultPassName);
		}

		return *mActivePass;
	}

	FRenderGraphPass &Renderer::BeginPass(const std::string &name, EPassType type)
	{
		auto &graph = GetActiveGraph();
		mActivePass = &graph.AddPass(name, type);

		if (mPassConfig.DebugMarkers)
			DebugPass("Begin Pass: " + name);

		return *mActivePass;
	}

	void Renderer::EndPass()
	{
		if (mActivePass && mPassConfig.DebugMarkers)
			DebugPass("EndPass: " + mActivePass->Name);

		mActivePass = nullptr;
	}

	void Renderer::SubmitResourceUpdate(FResourceUpdateList::UpdateCommand cmd)
	{
		mResourceUpdates.Push(std::move(cmd));
	}

	void Renderer::SetPassConfig(const PassConfig &config)
	{
		mPassConfig = config;
	}

	void Renderer::DebugPass(const std::string &msg)
	{
		LOG_TRACE(msg);
	}

} // namespace BHive