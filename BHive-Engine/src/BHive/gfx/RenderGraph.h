#pragma once

#include "core/Core.h"
#include "rendergraph/Pass.h"

namespace BHive
{
	class Texture;
	class Pipeline;
	class BaseMesh;
	class Material;

	class FResourceUpdateList
	{
	public:
		using UpdateCommand = std::function<void(IRendererContext&)>;

		void Push(UpdateCommand cmd);

		template <typename T, typename Method, typename... Args>
		void Push(Method method, Args &&... args)
		{
			UpdateCommand cmd = [method, ... captured = std::forward<Args>(args)](IRendererContext &ctx) { (*method)(ctx, captured...); };
			Push(std::move(cmd));
		}

		template<typename T, typename Method, typename... Args>
		void Push(T* obj, Method method, Args&&... args)
		{
			UpdateCommand cmd = [obj, method, ... captured = std::forward<Args>(args)](IRendererContext &ctx) { (obj->*method)(ctx, captured...); };
			Push(std::move(cmd));
		}

		void Append(FResourceUpdateList &updates);

		void Execute(IRendererContext &ctx) const;

		void Clear();

		bool Empty() const;



	private:
		std::vector<UpdateCommand> mUpdateCommands;
	};
	
	struct FComputeBindings
	{
		virtual ~FComputeBindings() = default;

		virtual void Bind(const char *name, Ref<Texture> tex, ImageSubresourceRange range = {}) = 0;

		virtual void Set(const char *name, const void* data, size_t size) = 0;

		template<typename T>
		void Set(const char* name, const T& data)
		{
			Set(name, &data, sizeof(T));
		}
	};

	struct FAsyncPass
	{
	public:

		virtual bool IsDone() = 0;

		virtual bool Wait() = 0;

		virtual void Destroy() = 0;

		virtual ~FAsyncPass() = default;
	};

	class RenderGraph
	{
	public:
	
		FPass &AddPass(const std::string &name, EPassType type);

		void Append(const RenderGraph &graph);

		bool Empty() const;

		const std::vector<FPass> &GetPasses() const;

		void DebugPrint();

	private:
		std::vector<FPass> mPasses;
	};

}