#pragma once

#include "core/Core.h"
#include "gfx/Enumerations.h"
#include "gfx/shader/ShaderProgram.h"
#include "IBindingGroup.h"

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
			virtual ~PipelineState() = default;

			enum Type
			{
				Graphics,
				Compute
			};

			virtual Type GetType() const = 0;

			virtual Scope<PipelineState> Clone() const = 0;
		};

		struct GraphicsPipelineState : public PipelineState
		{
			// VertexLayout Layout;
			ETopologyMode DrawMode{};
			RasterState Raster{};
			DepthState Depth{};
			BlendState Blend{};

			Type GetType() const override { return Graphics; }

			Scope<PipelineState> Clone() const override { return CreateScope<GraphicsPipelineState>(*this); }
		};

		struct ComputePipelineState : public PipelineState
		{
			Type GetType() const override { return Compute; }

			Scope<PipelineState> Clone() const override { return CreateScope<ComputePipelineState>(*this); }
		};

		virtual ~Pipeline() = default;

		virtual void Init(const PipelineState &state) = 0;

		static GraphicsPipelineState GetDefaultGraphicsPipelineState();

		static Ref<Pipeline> Create();
	};

	class BHIVE_API PipelineRegistry
	{
	public:
		PipelineRegistry() = default;
		PipelineRegistry(const PipelineRegistry &) = delete;

		static void Initialize();

		static void Register(const std::string &name, const Pipeline::GraphicsPipelineState &info);

		static void Register(const std::string &name, const Pipeline::ComputePipelineState &info);

		static Pipeline *Get(const std::string &name);

		static void Reload();

		static void Shutdown();

	private:
		struct Entry
		{
			std::variant<Pipeline::GraphicsPipelineState, Pipeline::ComputePipelineState> StateInfo;
			Ref<Pipeline> PipelineRef;
		};

	private:
		static inline std::unordered_map<std::string, Entry> mRegistry;
	};
} // namespace BHive