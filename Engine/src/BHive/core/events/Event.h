#pragma once

#include "core/Core.h"

#define EVENT(cls, flags)                 \
public:                                   \
	int GetCategoryFlags() const override \
	{                                     \
		return flags;                     \
	}                                     \
	uint64_t GetType() const override     \
	{                                     \
		return GetStaticType();           \
	}                                     \
	static uint64_t GetStaticType()       \
	{                                     \
		return typeid(cls).hash_code();   \
	}

namespace BHive
{
	enum EventCategory
	{
		EventCategoryNone = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4),
	};

	class BHIVE Event
	{
	public:
		virtual ~Event() = default;

		virtual uint64_t GetType() const = 0;

		virtual int GetCategoryFlags() const = 0;

		operator bool() const { return mHandled; }

		bool mHandled = false;

		bool IsInCategory(EventCategory category) { return GetCategoryFlags() & category; }
	};

	template <typename Derived, typename Base>
	using is_derived_from = std::enable_if_t<std::is_base_of_v<Base, Derived>>;

	class BHIVE EventDispatcher
	{
	public:
		EventDispatcher(Event &event)
			: mEvent(event)
		{
		}

		template <typename T, typename U, typename TEvent, typename = is_derived_from<T, U>>
		void Dispatch(T *instance, bool (U::*func)(TEvent &))
		{
			if (TEvent::GetStaticType() == mEvent.GetType() && !mEvent.mHandled)
			{
				mEvent.mHandled |= (instance->*func)(static_cast<TEvent &>(mEvent));
			}
		}

	private:
		Event &mEvent;
	};
} // namespace BHive