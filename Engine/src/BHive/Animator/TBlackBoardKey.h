#pragma once

#include "BlackBoardKey.h"
#include "core/Core.h"

namespace BHive
{
	template <typename T>
	class TBlackBoardKey : public BlackBoardKey
	{
	private:
		/* data */
	public:
		TBlackBoardKey(/* args */) = default;
		~TBlackBoardKey() = default;

		virtual std::any Get() const { return mValue; };

		virtual void Set(const std::any &value) { mValue = std::any_cast<T>(value); }

		virtual bool Compare(const std::any &value) const
		{
			return value.type() == typeid(T) && mValue == std::any_cast<T>(value);
		}

		REFLECTABLEV(BlackBoardKey)

	private:
		T mValue{};
	};

} // namespace BHive

#define REFLECT_BLACKBOARD_KEY(cls)                 \
	REFLECT(::BHive::TBlackBoardKey<cls>)           \
	{                                               \
		BEGIN_REFLECT(::BHive::TBlackBoardKey<cls>) \
		REFLECT_PROPERTY("Value", mValue);          \
	}

REFLECT_BLACKBOARD_KEY(int)
REFLECT_BLACKBOARD_KEY(float)
REFLECT_BLACKBOARD_KEY(bool)