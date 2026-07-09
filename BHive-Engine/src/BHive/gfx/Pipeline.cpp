#include "Pipeline.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "RenderCommand.h"
#include "ShaderManager.h"

namespace BHive
{
	Pipeline::GraphicsPipelineState Pipeline::GetDefaultGraphicsPipelineState()
	{
		GraphicsPipelineState state{};

		state.Raster.CullEnabled = true;
		state.Raster.CullMode = ECullMode::Back;
		state.Raster.FrontFace = EFrontFace::CWW;
		state.Raster.FillMode = EPolygonMode::Fill;

		state.DrawMode = ETopologyMode::Triangles;
		state.Blend.Enabled = true;
		state.Blend.SrcColor = EBlendFactor::One;
		state.Blend.DstColor = EBlendFactor::OneMinusSrcAlpha;
		state.Blend.ColorOp = EBlendOp::Add;
		state.Blend.SrcAlpha = EBlendFactor::One;
		state.Blend.DstAlpha = EBlendFactor::Zero;
		state.Blend.AlphaOp = EBlendOp::Add;

		state.Depth.DepthTest = true;
		state.Depth.DepthWrite = true;
		state.Depth.DepthCompare = ECompareOp::LessOrEqual;
		state.DepthAttachmentFormat = EFormat::DEPTH24_STENCIL8;

		state.ColorAttachmentFormats = {EFormat::RGBA32F};

		return state;
	}

	Ref<Pipeline> BHive::Pipeline::Create()
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::Vulkan:
			return CreateRef<VulkanPipeline>();
		}

		ASSERT(false)
		return nullptr;
	}

	void PipelineRegistry::Initialize()
	{

		{
			auto state = Pipeline::GetDefaultGraphicsPipelineState();
			state.ColorAttachmentFormats = {EFormat::RGBA8};
			state.ShaderProgram = ShaderManager::Get("ColorGrading.glsl");
			Register("COLOR_GRADING", state);
		}

		{
			auto state = Pipeline::GetDefaultGraphicsPipelineState();
			state.ColorAttachmentFormats = {EFormat::RGBA8};
			state.ShaderProgram = ShaderManager::Get("Aces.glsl");
			PipelineRegistry::Register("ACES", state);
		}

		Pipeline::ComputePipelineState state{};

		state.ShaderProgram = ShaderManager::Get("CombineTex.glsl");
		PipelineRegistry::Register("BLOOM_COMBINE", state);

		state.ShaderProgram = ShaderManager::Get("PreFilter.glsl");
		PipelineRegistry::Register("BLOOM_PREFILTER", state);

		state.ShaderProgram = ShaderManager::Get("DownSample.glsl");
		PipelineRegistry::Register("BLOOM_DOWNSAMPLE", state);

		state.ShaderProgram = ShaderManager::Get("UpSample.glsl");
		PipelineRegistry::Register("BLOOM_UPSAMPLE", state);
	}

	void PipelineRegistry::Register(const std::string &name, const Pipeline::GraphicsPipelineState &info)
	{
		Entry entry;
		entry.StateInfo = info;
		mRegistry[name] = entry;
	}

	void PipelineRegistry::Register(const std::string &name, const Pipeline::ComputePipelineState &info)
	{
		Entry entry;
		entry.StateInfo = info;
		mRegistry[name] = entry;
	}

	Pipeline *PipelineRegistry::Get(const std::string &name)
	{
		auto &entry = mRegistry[name];
		if (!entry.PipelineRef)
		{
			entry.PipelineRef = Pipeline::Create();
			std::visit([&](auto &&state) { entry.PipelineRef->Init(state); }, entry.StateInfo);
		}

		return entry.PipelineRef.get();
	}

	void PipelineRegistry::Reload()
	{
		for (auto &[name, entry] : mRegistry)
		{
			entry.PipelineRef = Pipeline::Create();
			std::visit([&](auto &&state) { entry.PipelineRef->Init(state); }, entry.StateInfo);
		}
	}

	void PipelineRegistry::Shutdown()
	{
		for (auto &[name, entry] : mRegistry)
		{
			entry.PipelineRef.reset();
		}
	}
} // namespace BHive