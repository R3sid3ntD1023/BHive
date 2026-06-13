#pragma once

#include "core/Core.h"
#include "Enumerations.h"
#include "renderers/ViewSystem.h"
#include "core/math/Transform.h"

namespace BHive
{
	class Texture;
	class Pipeline;
	class BaseMesh;
	class Material;

	struct IRendererContext
	{
		virtual ~IRendererContext() = default;

		template<typename TRendererContext>
			requires(std::is_base_of_v<IRendererContext, TRendererContext>)
		TRendererContext &As()
		{
			ASSERT(dynamic_cast<TRendererContext *>(this) != nullptr)
			return static_cast<TRendererContext &>(*this);
		}
	};

	class FResourceUpdateList
	{
	public:
		using UpdateCommand = std::function<void(IRendererContext&)>;

		void Push(UpdateCommand cmd);

		void Append(FResourceUpdateList &updates);

		void Execute(IRendererContext &ctx) const;

		void Clear();

		bool Empty() const;

	private:
		std::vector<UpdateCommand> mUpdateCommands;
	};

	struct FRenderCommandList
	{
		using RenderCommand = std::function<void(IRendererContext &)>;

		void Push(const std::string &name, RenderCommand cmd);

		void Execute(IRendererContext &ctx) const;

	private:
		struct FEntry
		{
			std::string Name;
			RenderCommand Func;
		};

		std::vector<FEntry> mCommands;
	};

	enum class EPassType : uint8_t
	{
		OffScreen,
		SwapChain,
		Viewport
	};

	//Tex, Mip, Levels, Layer, Layers, Access
	struct FImageInfo
	{
		Ref<Texture> Texture;
		uint32_t BaseMip = 0;
		uint32_t LevelCount = 1;
		uint32_t BaseLayer = 0;
		uint32_t LayerCount = 1;
		EImageAccess Access = EImageAccess::WRITE;
	};

	
	struct FComputeBindings
	{
		virtual void StorageImage(const char *name, const FImageInfo& info) = 0;

		virtual void SampledImage(const char *name, const FImageInfo &info) = 0;

		virtual void Set(const char *name, const void* data, size_t size) = 0;

		template<typename T>
		void Set(const char* name, const T& data)
		{
			Set(name, &data, sizeof(T));
		}
	};

	struct FRenderGraphPass
	{
		std::string Name;
		EPassType Type{};
		FRenderCommandList CommandList{};
		std::vector<FImageInfo> Images{};
		std::optional<FView> View;

		const FView& GetView() const { return View.value(); }

		bool HasView() const { return View.has_value(); }

	};

	struct FAsyncPass
	{
	public:

		virtual bool IsDone() = 0;

		virtual void Wait() = 0;

		virtual void Destroy() = 0;

		virtual ~FAsyncPass() = default;
	};

	class RenderGraph
	{
	public:
	
		FRenderGraphPass &AddPass(const std::string &name, EPassType type);

		void Append(const RenderGraph &graph);

		bool Empty() const;

		const std::vector<FRenderGraphPass> &GetPasses() const;

	private:
		std::vector<FRenderGraphPass> mPasses;
	};

}