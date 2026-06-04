#include "Pipeline.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "RenderCommand.h"

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

		state.ColorAttachmentFormats = {EFormat::RGBA8};

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

	void PipelineRegistry::Register(const std::string &name, const Pipeline::GraphicsPipelineState &info)
	{
		Entry entry;
		entry.Info = info;
		mRegistry[name] = entry;
	}

	void PipelineRegistry::Register(const std::string &name, const Pipeline::ComputePipelineState &info)
	{
		Entry entry;
		entry.Info = info;
		mRegistry[name] = entry;
	}

	Pipeline *PipelineRegistry::Get(const std::string &name)
	{
		auto &entry = mRegistry[name];
		if (!entry.mPipeline)
		{
			entry.mPipeline = Pipeline::Create();

			std::visit([&](auto &&state) { entry.mPipeline->Init(state);
				}, entry.Info);
		}

		return entry.mPipeline.get();
	}

	void PipelineRegistry::Reload()
	{
		for (auto& [name, entry] : mRegistry)
		{
			entry.mPipeline = Pipeline::Create();
			std::visit([&](auto &&state) { entry.mPipeline->Init(state);
				}, entry.Info);
		}
	}

	void PipelineRegistry::Shutdown()
	{
		for (auto &[name, entry] : mRegistry)
		{
			entry.mPipeline.reset();
		}
	}
} // namespace BHive