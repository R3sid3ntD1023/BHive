#pragma once

#include "core/Core.h"
#include "gfx/Enumerations.h"
#include "gfx/shader/ShaderProgram.h"

namespace BHive
{
	class BHIVE_API Pipeline
	{
	public:
		struct RasterState
		{
			bool CullEnabled;
			ECullMode CullMode;
			EFrontFace FrontFace;
			EPolygonMode FillMode;
		};

		struct DepthState
		{
			bool DepthTest;
			bool DepthWrite;
			ECompareOp DepthCompare;
		};

		struct BlendState
		{
			bool Enabled;
			EBlendFactor SrcColor;
			EBlendFactor DstColor;
			EBlendOp ColorOp;
			EBlendFactor SrcAlpha;
			EBlendFactor DstAlpha;
			EBlendOp AlphaOp;
		};

		struct PipelineState
		{
			Ref<ShaderProgram> ShaderProgram;
		};

		struct GraphicsPipelineState : public PipelineState
		{
			//VertexLayout Layout;
			ETopologyMode DrawMode{};
			RasterState Raster{};
			DepthState Depth{};
			BlendState Blend{};
			std::vector<EFormat> ColorAttachmentFormats{};
			EFormat DepthAttachmentFormat{};
		};

		struct ComputePipelineState : public PipelineState
		{
		};

		virtual ~Pipeline() = default;

		virtual void Init(const GraphicsPipelineState& state) = 0;

		virtual void Init(const ComputePipelineState &state) = 0;

		virtual void Bind() = 0;

		virtual void UnBind() = 0;

		virtual Ref<ShaderProgram> GetShaderProgram() const = 0;

		static GraphicsPipelineState GetDefaultGraphicsPipelineState();

		static Ref<Pipeline> Create();
	};

	class BHIVE_API PipelineRegistry
	{
	public:
		PipelineRegistry() = default;
		PipelineRegistry(const PipelineRegistry &) = delete;

		static void Register(const std::string &name, const Pipeline::GraphicsPipelineState &info);

		static void Register(const std::string &name, const Pipeline::ComputePipelineState &info);

		static Pipeline *Get(const std::string &name);

		static void Reload();

		static void Shutdown();

	private:
		struct Entry
		{
			std::variant<Pipeline::GraphicsPipelineState, Pipeline::ComputePipelineState> Info;
			Ref<Pipeline> mPipeline;
		};

	private:
		static inline std::unordered_map<std::string, Entry> mRegistry;
	};
} // namespace BHive