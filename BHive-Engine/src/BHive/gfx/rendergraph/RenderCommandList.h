#pragma once

#include "RenderContext.h"

namespace BHive
{
	struct FCommand
	{
		std::function<void(IRendererContext&)> Fn;
	};

	template<typename T>
	constexpr size_t LambdaSize(const T &)
	{
		return sizeof(T);
	}

	struct FRenderCommandList
	{
		void PushInternal(const std::string &name, FCommand &&cmd);

		template <typename Callable>
		void Push(const std::string &name, Callable&& fn)
		{
			FCommand c{std::forward<Callable>(fn)};
			PushInternal(name, std::move(c));
		}

		template <typename T, typename Method, typename... Args>
		void Push(const std::string &name, T *obj, Method method, Args &&...args)
		{
			auto lambda = [obj, method, ... captured = std::forward<Args>(args)](IRendererContext &ctx) { (obj->*method)(ctx, captured...); };

			static_assert(LambdaSize(lambda) <= 32, "Lambda too big for SBO!");

			FCommand c{lambda};
			PushInternal(name, std::move(c));
		}

		void Execute(IRendererContext &ctx) const;

		const auto &GetCommands() const { return mCommands; }

	private:
		struct FEntry
		{
			std::string Name;
			FCommand Cmd;
		};

		std::vector<FEntry> mCommands;
	};
}