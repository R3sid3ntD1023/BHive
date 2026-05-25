#pragma once

#include "core/Core.h"

namespace BHive
{
	class Texture;

	struct IRendererContext
	{
		virtual ~IRendererContext() = default;
	};

	class FResourceUpdateList
	{
	public:
		using UpdateCommand = std::function<void(IRendererContext&)>;

		void Push(UpdateCommand cmd) { mUpdateCommands.push_back(std::move(cmd));}

		void Append(FResourceUpdateList &updates) { mUpdateCommands.insert(mUpdateCommands.end(), updates.mUpdateCommands.begin(), updates.mUpdateCommands.end()); }

		void Execute(IRendererContext & ctx) const
		{
			for (auto& cmd : mUpdateCommands)
			{
				cmd(ctx);
			}
		}

		bool Empty() const { return mUpdateCommands.empty(); }

	private:
		std::vector<UpdateCommand> mUpdateCommands;
	};

	struct FRenderCommandList
	{
		using RenderCommand = std::function<void(IRendererContext &)>;

		void Push(const std::string &name, RenderCommand cmd) { mCommands.push_back({name, std::move(cmd)}); }

		void Execute(IRendererContext &ctx) const
		{
			
			for (auto& cmd : mCommands)
			{
				//LOG_INFO(cmd.Name);
				cmd.Func(ctx);
			}
				
		}

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
		Compute,
		Transfer
	};

	struct FPassImage
	{
		Ref<Texture> Texture;
		uint32_t BaseMip = 0;
		uint32_t LevelCount = 1;
		uint32_t BaseLayer = 0;
		uint32_t LayerCount = 1;

		EImageAccess Access = EImageAccess::WRITE;
	};

	struct FRenderGraphPass
	{
		std::string Name;
		EPassType Type;
		FRenderCommandList CommandList;
		std::vector<FPassImage> Images;
	};

	struct FComputeBindings
	{
		virtual void StorageImage(const char *name, const Ref<Texture>& tex, uint32_t mip = 0) = 0;

		virtual void SampledImage(const char *name, const Ref<Texture> &tex, uint32_t mip = 0) = 0;

		virtual void Set(const char *name, const void* data, size_t size) = 0;

		template<typename T>
		void Set(const char* name, const T& data)
		{
			Set(name, &data, sizeof(T));
		}
	};

	class RenderGraph
	{
	public:
	
		FRenderGraphPass &AddPass(const std::string &name, EPassType type)
		{
			auto &pass = mPasses.emplace_back();
			pass.Name = name;
			pass.Type = type;
			return pass;
		}

		void Append(const RenderGraph &graph)
		{
			auto& passes = graph.GetPasses();
			mPasses.insert(mPasses.end(), passes.begin(), passes.end());
		}

		bool Empty() const { return mPasses.empty(); }

		const std::vector<FRenderGraphPass> &GetPasses() const { return mPasses; }

	private:
		std::vector<FRenderGraphPass> mPasses;
	};

}