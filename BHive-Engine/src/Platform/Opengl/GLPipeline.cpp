#include "GLPipeline.h"
#include <glad/glad.h>

namespace BHive
{
	namespace utils
	{
		GLenum ToCull(ECullMode m)
		{
			switch (m)
			{
			case ECullMode::Front:
				return GL_FRONT;
			case ECullMode::Back:
				return GL_BACK;
			case ECullMode::FrontAndBack:
				return GL_FRONT_AND_BACK;
			default:
				return GL_BACK;
			}
		}

		GLenum ToFrontFace(EFrontFace f)
		{
			switch (f)
			{
			case EFrontFace::CW:
				return GL_CW;
			case EFrontFace::CCW:
				return GL_CCW;
			}
			return GL_CCW;
		}

		GLenum ToPolygonMode(EPolygonMode p)
		{
			switch (p)
			{
			case EPolygonMode::Fill:
				return GL_FILL;
			case EPolygonMode::Line:
				return GL_LINE;
			case EPolygonMode::Point:
				return GL_POINT;
			}
			return GL_FILL;
		}

		GLenum ToCompare(ECompareOp op)
		{
			switch (op)
			{
			case ECompareOp::Never:
				return GL_NEVER;
			case ECompareOp::Less:
				return GL_LESS;
			case ECompareOp::Equal:
				return GL_EQUAL;
			case ECompareOp::LessOrEqual:
				return GL_LEQUAL;
			case ECompareOp::Greater:
				return GL_GREATER;
			case ECompareOp::NotEqual:
				return GL_NOTEQUAL;
			case ECompareOp::GreaterOrEqual:
				return GL_GEQUAL;
			case ECompareOp::Always:
				return GL_ALWAYS;
			}
			return GL_ALWAYS;
		}

		GLenum ToBlendFactor(EBlendFactor factor)
		{
			switch (factor)
			{
			case EBlendFactor::Zero:
				return GL_ZERO;
			case EBlendFactor::One:
				return GL_ONE;
			case EBlendFactor::SrcColor:
				return GL_SRC_COLOR;
			case EBlendFactor::OneMinusSrcColor:
				return GL_ONE_MINUS_SRC_COLOR;
			case EBlendFactor::DstColor:
				return GL_DST_COLOR;
			case EBlendFactor::OneMinusDstColor:
				return GL_ONE_MINUS_DST_COLOR;
			case EBlendFactor::SrcAlpha:
				return GL_SRC_ALPHA;
			case EBlendFactor::OneMinusSrcAlpha:
				return GL_ONE_MINUS_SRC_ALPHA;
			case EBlendFactor::DstAlpha:
				return GL_DST_ALPHA;
			case EBlendFactor::OneMinusDstAlpha:
				return GL_ONE_MINUS_DST_ALPHA;
			}
			return GL_ONE;
		}

		GLenum ToBlendOp(EBlendOp op)
		{
			switch (op)
			{
			case EBlendOp::Add:
				return GL_FUNC_ADD;
			case EBlendOp::Subtract:
				return GL_FUNC_SUBTRACT;
			case EBlendOp::ReverseSubtract:
				return GL_FUNC_REVERSE_SUBTRACT;
			case EBlendOp::Min:
				return GL_MIN;
			case EBlendOp::Max:
				return GL_MAX;
			}
			return GL_FUNC_ADD;
		}

		bool SetEnabled(GLenum v, bool enabled)
		{
			enabled ? glEnable(v) : glDisable(v);
			return enabled;
		}
	} // namespace utils

	void GLPipeline::Init(const PipelineState *state)
	{
		mState = state->Clone();
	}

	void GLPipeline::Bind(uint32_t numAttachments)
	{
		// bind states
		if (mState->GetType() == Pipeline::PipelineState::Graphics)
		{
			auto state = Cast<GraphicsPipelineState>(mState.get());

			const bool cullEnabled = state->Raster.CullEnabled && state->Raster.CullMode != ECullMode::None;
			if (utils::SetEnabled(GL_CULL_FACE, cullEnabled))
			{
				auto face = utils::ToFrontFace(state->Raster.FrontFace);
				auto poly = utils::ToPolygonMode(state->Raster.FillMode);
				auto cull = utils::ToCull(state->Raster.CullMode);

				glCullFace(cull);
				glFrontFace(face);

				// glPolygonMode(GL_FRONT_AND_BACK, poly);
			}
			else
			{
				// glPolygonMode(GL_FRONT_AND_BACK, utils::ToPolygonMode(state->Raster.FillMode));
			}

			if (utils::SetEnabled(GL_DEPTH_TEST, state->Depth.DepthTest))
			{
				glDepthFunc(utils::ToCompare(state->Depth.DepthCompare));
			}
			glDepthMask(state->Depth.DepthWrite ? GL_TRUE : GL_FALSE);

			utils::SetEnabled(GL_POLYGON_OFFSET_FILL, state->Depth.DepthBias);
			if (state->Depth.DepthBias)
				glPolygonOffset(state->Depth.SlopeFactor, state->Depth.ConstantFactor);

			utils::SetEnabled(GL_STENCIL_TEST, true);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
			glStencilMask(0xFFFFFFFF);

			glDisable(GL_RASTERIZER_DISCARD);
			utils::SetEnabled(GL_SAMPLE_ALPHA_TO_COVERAGE, numAttachments != 0);

			if (numAttachments != 0)
			{
				for (uint32_t i = 0; i < numAttachments; i++)
				{
					glColorMaski(i, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				}

				if (utils::SetEnabled(GL_BLEND, state->Blend.Enabled))
				{
					glBlendFuncSeparate(
						utils::ToBlendFactor(state->Blend.SrcColor),
						utils::ToBlendFactor(state->Blend.DstColor),
						utils::ToBlendFactor(state->Blend.SrcAlpha),
						utils::ToBlendFactor(state->Blend.DstAlpha)
					);
					glBlendEquationSeparate(utils::ToBlendOp(state->Blend.ColorOp), utils::ToBlendOp(state->Blend.AlphaOp));
				}
			}
			else
			{
				utils::SetEnabled(GL_BLEND, false);
			}

			glDisable(GL_PRIMITIVE_RESTART);
		}
	}
} // namespace BHive