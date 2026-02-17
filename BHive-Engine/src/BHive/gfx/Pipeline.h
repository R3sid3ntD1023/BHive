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
			//VertexLayout Layout;
			ETopologyMode DrawMode;
			RasterState Raster;
			DepthState Depth;
			BlendState Blend;
			std::vector<EFormat> ColorAttachmentFormats;
			EFormat DepthAttachmentFormat = EFormat::None;
		};

		virtual ~Pipeline() = default;

		virtual void Init(const PipelineState& state) = 0;

		virtual void Bind() = 0;

		virtual void UnBind() = 0;

		virtual Ref<ShaderProgram> GetShaderProgram() const = 0;

		static PipelineState GetDefaultPipelineState();

		static Ref<Pipeline> Create();
	};
} // namespace BHive