#include "Renderer.h"
#include "gfx/RenderCommand.h"
#include "gfx/Texture.h"
#include "gfx/factories/TextureFactory.h"
#include "gfx/registries/RegistryManager.h"

namespace BHive
{

	struct BHIVE_API RenderData
	{
		TexturePtr WhiteTexture;
		TexturePtr BlackTexture;
		TexturePtr BlueTexture;

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

			WhiteTexture = TextureFactory::Create2D({1, 1}, create_info, ByteBuffer(&white, sizeof(uint32_t)));

			create_info.DebugName = "Black Texture";
			BlackTexture = TextureFactory::Create2D({1, 1}, create_info, ByteBuffer(&black, sizeof(uint32_t)));

			create_info.DebugName = "Blue Texture";
			BlueTexture = TextureFactory::Create2D({1, 1}, create_info, ByteBuffer(&blue, sizeof(uint32_t)));
		}
	};

	const BindingSetTemplate &RendererTemplates::Global()
	{
		static auto globalSetTemplate = []() -> BindingSetTemplate
		{
			BindingSetTemplate t{};
			t.SetIndex = ERenderingSets::GLOBAL;
			t.Bindings.emplace_back(0, 0u, 1, EShaderStage::Vertex | EShaderStage::Fragment | EShaderStage::Compute, EResourceType::UniformBuffer); // camera
			t.BuildLayoutHash();
			return t;
		}();

		return globalSetTemplate;
	}

	const BindingSetTemplate &RendererTemplates::Object()
	{
		static auto objectSetTemplate = []() -> BindingSetTemplate
		{
			BindingSetTemplate t{};
			t.SetIndex = ERenderingSets::OBJECT;
			t.Bindings.emplace_back(0, 0u, 1, EShaderStage::Compute | EShaderStage::Vertex, EResourceType::StorageBuffer); // objectdata
			t.Bindings.emplace_back(1, 0u, 1, EShaderStage::Compute | EShaderStage::Vertex, EResourceType::StorageBuffer); // indirect
			t.Bindings.emplace_back(2, 0u, 1, EShaderStage::Compute | EShaderStage::Vertex, EResourceType::StorageBuffer); // visibility
			t.Bindings.emplace_back(3, 0u, 1, EShaderStage::Vertex, EResourceType::StorageBuffer);						   // bones
			t.BuildLayoutHash();
			return t;
		}();

		return objectSetTemplate;
	}

	const BindingSetTemplate &RendererTemplates::Lighting()
	{
		static auto t = []() -> BindingSetTemplate
		{
			BindingSetTemplate t{};
			t.SetIndex = ERenderingSets::LIGHTING;
			t.Bindings.emplace_back(0, 0u, 1, EShaderStage::Fragment | EShaderStage::Compute, EResourceType::StorageBuffer); // DirectionalLights
			t.Bindings.emplace_back(1, 0u, 1, EShaderStage::Fragment | EShaderStage::Compute, EResourceType::StorageBuffer); // LocalLights
			t.BuildLayoutHash();
			return t;
		}();

		return t;
	}

	const BindingSetTemplate &RendererTemplates::Shadow()
	{
		static auto t = []() -> BindingSetTemplate
		{
			BindingSetTemplate t{};
			t.SetIndex = ERenderingSets::SHADOW;
			t.Bindings.emplace_back(
				0, 0u, 1, EShaderStage::Vertex | EShaderStage::Geometry | EShaderStage::Fragment | EShaderStage::Compute, EResourceType::StorageBuffer
			);																								// shadowBuffer
			t.Bindings.emplace_back(1, 0u, 1, EShaderStage::Fragment, EResourceType::CombinedImageSampler); // Dir shadow
			t.Bindings.emplace_back(2, 0u, 1, EShaderStage::Fragment, EResourceType::CombinedImageSampler); // Point shadow
			t.Bindings.emplace_back(3, 0u, 1, EShaderStage::Fragment, EResourceType::CombinedImageSampler); // Spot shadow
			t.BuildLayoutHash();
			return t;
		}();

		return t;
	}

	const BindingSetTemplate &RendererTemplates::ImageLighting()
	{
		static auto t = []() -> BindingSetTemplate
		{
			BindingSetTemplate t{};
			t.SetIndex = ERenderingSets::IMAGE_BASED_LIGHTING;
			t.Bindings.emplace_back(0, 0u, 1, EShaderStage::Fragment, EResourceType::CombinedImageSampler); // brdfLUT
			t.Bindings.emplace_back(1, 0u, 1, EShaderStage::Fragment, EResourceType::CombinedImageSampler); // prefilter
			t.Bindings.emplace_back(2, 0u, 1, EShaderStage::Fragment, EResourceType::CombinedImageSampler); // irradiance
			t.BuildLayoutHash();
			return t;
		}();

		return t;
	}

	const std::vector<BindingSetTemplate> &RendererTemplates::GetTemplates()
	{
		static auto m = []()
		{
			std::vector<BindingSetTemplate> m = {Global(), Object(), Lighting(), Shadow(), ImageLighting()};
			return m;
		}();

		return m;
	}

	Renderer::Renderer(Scope<RendererAPI> api)
		: mAPI(std::move(api))
	{
		ASSERT(mAPI);

		sInstance = this;

		mAPI->Init();

		ResourceRegistriesManager::Init();

		mData = CreateRef<RenderData>();

		Line.Initialize();
		Quad.Initialize();
	}

	Renderer::~Renderer()
	{
		ResourceRegistriesManager::Shutdown();

		mData.reset();
		mAPI->Shutdown();
	}

	void Renderer::BeginFrame()
	{
		mGraph = Graph{};
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

	void Renderer::ExecuteGraph(Graph &graph)
	{
		mAPI->SubmitGraph(graph);
	}

	void Renderer::ResetStats()
	{
		memset(&mStats, 0, sizeof(Statitics));
	}

	Texture2DPtr Renderer::GetWhiteTexture() const
	{
		return mData->WhiteTexture;
	}

	Graph &Renderer::GetActiveGraph()
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

} // namespace BHive