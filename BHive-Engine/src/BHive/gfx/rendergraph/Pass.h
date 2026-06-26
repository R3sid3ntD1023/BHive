#pragma once


#include "gfx/Enumerations.h"
#include "gfx/resources/ImageSubResourceRange.h"
#include "gfx/renderers/ViewSystem.h"
#include "Phase.h"

namespace BHive
{
	struct IRenderContext;

	enum class EPassType : uint8_t
	{
		OffScreen,
		SwapChain,
		Viewport
	};

	struct BHIVE_API FPass
	{
		std::string Name;
		EPassType Type{};
		std::vector<FPhase> Phases;
		std::optional<FView> View;

		void BeginPhase();

		void BeginPhase(const std::string &name);

		template <typename Callable>
		void Push(const std::string &name, Callable &&fn)
		{
			Phases[mCurrentPhase].CommandList.Push(name, std::move(fn));
		}

		template <typename T, typename Method, typename... Args>
			requires(std::is_member_function_pointer_v<Method> && std::is_invocable_r_v<void, Method, T *, IRendererContext &, Args...>)
		void Push(const std::string &name, T *obj, Method method, Args &&...args)
		{
			Phases[mCurrentPhase].CommandList.Push(name, obj, method, std::forward<Args>(args)...);
		}

		void Push(Ref<Texture> tex, EImageAccess access, ImageSubresourceRange range = {});

		void EndPhase();

		const FView &GetView() const { return View.value(); }
		bool HasView() const { return View.has_value(); }

	private:
		int32_t mCurrentPhase;
	};
}