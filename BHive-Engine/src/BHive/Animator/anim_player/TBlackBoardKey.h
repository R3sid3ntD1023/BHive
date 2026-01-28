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

		virtual bool Compare(const std::any &value) const { return value.type() == typeid(T) && mValue == std::any_cast<T>(value); }

		REFLECTABLEV(BlackBoardKey)

	protected:
		T mValue{};
	};

	struct BlackBoardKeyInt : public TBlackBoardKey<int32_t>
	{
		REFLECTABLEV(TBlackBoardKey)
	};

	struct BlackBoardKeyFloat : public TBlackBoardKey<float>
	{
		REFLECTABLEV(TBlackBoardKey)
	};

	struct BlackBoardKeyBool : public TBlackBoardKey<bool>
	{
		REFLECTABLEV(TBlackBoardKey)
	};

	REFLECT_EXTERN(BlackBoardKeyInt)
	REFLECT_EXTERN(BlackBoardKeyFloat)
	REFLECT_EXTERN(BlackBoardKeyBool)

} // namespace BHive
