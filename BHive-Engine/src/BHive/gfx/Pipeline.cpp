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
		state.Blend.SrcColor = EBlendFactor::SrcAlpha;
		state.Blend.DstColor = EBlendFactor::OneMinusSrcAlpha;
		state.Blend.ColorOp = EBlendOp::Add;
		state.Blend.SrcAlpha = EBlendFactor::One;
		state.Blend.DstAlpha = EBlendFactor::OneMinusSrcAlpha;
		state.Blend.AlphaOp = EBlendOp::Add;

		state.Depth.DepthTest = true;
		state.Depth.DepthWrite = true;
		state.Depth.DepthCompare = ECompareOp::LessOrEqual;

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
			auto defaultState = Pipeline::GetDefaultGraphicsPipelineState();
			Register("DEFAULT", defaultState);
		}

		{
			auto state = Pipeline::GetDefaultGraphicsPipelineState();
			state.Blend.Enabled = false;
			state.Depth.DepthTest = true;
			state.Depth.DepthWrite = true;
			Register("MESH_OPAQUE", state);
		}

		{
			auto state = Pipeline::GetDefaultGraphicsPipelineState();
			state.Blend.Enabled = false;
			state.Depth.DepthTest = true;
			state.Depth.DepthWrite = true;
			Register("MESH_MASKED", state);
		}

		{
			auto state = Pipeline::GetDefaultGraphicsPipelineState();
			state.Blend.Enabled = true;
			state.Depth.DepthTest = true;
			state.Depth.DepthWrite = false;
			Register("MESH_TRANSPARENT", state);
		}

		{
			auto state = Pipeline::GetDefaultGraphicsPipelineState();
			state.Depth.DepthTest = true;
			state.Depth.DepthWrite = true;
			Register("MESH_SHADOW", state);
		}
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