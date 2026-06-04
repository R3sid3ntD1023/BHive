#include "buffers/ModelBuffer.h"
#include "buffers/LightBuffer.h"
#include "gfx/Texture.h"
#include "Renderer.h"
#include "gfx/RenderCommand.h"
#include "gfx/Buffers.h"

namespace BHive
{
	struct BHIVE_API FObjectData
	{
		glm::mat4 WorldMatrix = {1.0f};
	};

	struct BHIVE_API RenderData
	{
		FCameraData CameraData;
		Frustum CameraFrustum;
		FModelBuffer ModelBuffer;
		LightBuffer LightingBuffer;

		Ref<Texture> WhiteTexture;
		Ref<Texture> BlackTexture;
		Ref<Texture> BlueTexture;

		Ref<GPUBuffer> CameraUniformBuffer;

		RenderData()
		{
			auto& global = AddSubSystem<GlobalBuffers>();

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

			CameraUniformBuffer = GPUBuffer::Create(sizeof(FCameraData), EBufferType::UniformBuffer);

			global.Register(0, CameraUniformBuffer);

			ModelBuffer.Init();
			LightingBuffer.Init();
		}

		~RenderData() { RemoveSubSystem<GlobalBuffers>();
		}
	};

	Renderer::Renderer(Scope<RendererAPI> api)
		: mAPI(std::move(api))
	{
		ASSERT(mAPI);

		sInstance = this;

		mData = CreateRef<RenderData>();

		mAPI->Init();

		Line.Initialize();
		Quad.Initialize();

		auto brdfLUT = mPMREMGenerator.GenerateBRDFLUTMap();
		mGlobalBuffers.Register(0, brdfLUT);
	}

	Renderer::~Renderer()
	{
		mAPI->Shutdown();
	}

	void Renderer::BeginFrame()
	{
		ResetStats();

		Line.BeginRecording();
		Quad.BeginRecording();

		mGraph = RenderGraph{};
		mActivePass = nullptr;
		mFrameActive = true;

		mResourceUpdates.Clear();
	}

	void Renderer::SubmitCamera(const glm::mat4 &projection, const glm::mat4 &view)
	{
		mData->CameraData.Projection = projection;
		mData->CameraData.View = view;
		mData->CameraData.NearFar.x = projection[3][2] / (projection[2][2] - 1.0f);
		mData->CameraData.NearFar.y = projection[3][2] / (projection[2][2] + 1.0f);
		mData->CameraData.Position = glm::inverse(view)[3];
		mData->CameraUniformBuffer->SetData(&mData->CameraData, sizeof(FCameraData));
		mData->CameraFrustum.Update(projection, view);

	}

	void Renderer::Flush()
	{
		Line.Flush(*this);
		Quad.Flush(*this);
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

		mGlobalBuffers.Register(1, cube_map);
		mGlobalBuffers.Register(2, irradiance);
		mGlobalBuffers.Register(3, prefilter);
	}

	Ref<Texture> Renderer::GetWhiteTexture()
	{
		return mData->WhiteTexture;
	}

	Ref<Texture> Renderer::GetBlackTexture()
	{
		return mData->BlackTexture;
	}

	void Renderer::ResetStats()
	{
		memset(&mStats, 0, sizeof(Statitics));
	}

	FModelBuffer &Renderer::GetModelBuffer()
	{
		return mData->ModelBuffer;
	}

	Ref<Texture> Renderer::GetPreFilterEnvironmentTexture()
	{
		return mGlobalBuffers.GetTextures().at(3);
	}

	Ref<Texture> Renderer::GetEnviromentCubeTexture()
	{
		return mGlobalBuffers.GetTextures().at(1);
	}

	Ref<Texture> Renderer::GetIrradianceTexture()
	{
		return mGlobalBuffers.GetTextures().at(2);
	}

	Ref<Texture> Renderer::GetBRDFLUTTexture()
	{
		return mGlobalBuffers.GetTextures().at(0);
	}

	const FCameraData &Renderer::GetCameraData() const
	{
		return mData->CameraData;
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

	void Renderer::MultiDrawElementsIndirect(ETopologyMode mode, BufferBase* indirect, VertexArray* vao, size_t drawCount, size_t stride)
	{
		auto pass = &GetActivePass();
		mAPI->MultiDrawElementsIndirect(pass, mode, indirect, vao, drawCount, stride);
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