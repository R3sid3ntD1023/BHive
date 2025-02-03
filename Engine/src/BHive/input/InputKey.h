#pragma once

#include "Inputs.h"
#include "core/reflection/Reflection.h"
#include "core/serialization/Serialization.h"

namespace BHive
{
	struct FInputKey
	{
		FInputKey() = default;

		void SetKey(EKey key)
		{
			mKey = key;
			mIsMouse = FKeyDetails(key).mIsMouse;
		}

		EKey GetKey() const { return mKey; }

		template <typename A>
		void Serialize(A &ar)
		{
			ar(mKey, mIsMouse);
		}

		operator uint32_t() const { return (uint32_t)mKey; }

		REFLECTABLE()

	private:
		EKey mKey{EKey::MouseButton1};

		bool mIsMouse{false};

		friend class FInputAction;
	};

	REFLECT(FInputKey)
	{
		BEGIN_REFLECT(FInputKey)
		REFLECT_PROPERTY("Key", GetKey, SetKey)
		REFLECT_PROPERTY_READ_ONLY("IsMouse", mIsMouse);
	}

} // namespace BHive
