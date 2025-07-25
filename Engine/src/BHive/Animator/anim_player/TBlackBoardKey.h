#pragma once

#include "BlackBoardKey.h"
#include "core/Core.h"

#define REFLECT_BLACKBOARD_KEY(cls, name)        \
	REFLECT(TBlackBoardKey<cls>)                 \
	{                                            \
		BEGIN_REFLECT(TBlackBoardKey<cls>, name) \
		REFLECT_CONSTRUCTOR()                    \
		REFLECT_PROPERTY("Value", mValue);       \
	}

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

	REFLECT_BLACKBOARD_KEY(int, "IntKey")
	REFLECT_BLACKBOARD_KEY(float, "FloatKey")
	REFLECT_BLACKBOARD_KEY(bool, "BoolKey")

} // namespace BHive
