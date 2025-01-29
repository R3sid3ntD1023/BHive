#pragma once

#include "core/Core.h"
#include "InputKey.h"
#include "reflection/Reflection.h"
#include "serialization/Serialization.h"

namespace BHive
{
	struct FInputAction
	{
		FInputAction() = default;

		FInputAction(const std::string &name, const FInputKey &key)
			: mName(name),
			  mKey(key)
		{
		}

		FInputKey GetKey() const { return mKey; }

		bool IsMouse() const { return mKey.mIsMouse; }

		bool IsKey() const { return !mKey.mIsMouse; }

		const std::string &GetName() const { return mName; }

		template <typename A>
		void Serialize(A &ar)
		{
			ar(mName, mKey);
		}

	private:
		std::string mName;
		FInputKey mKey;

		REFLECTABLE()

		friend class InputContext;
	};

	REFLECT_EXTERN(FInputAction)

} // namespace BHive
