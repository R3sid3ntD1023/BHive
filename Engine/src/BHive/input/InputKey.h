#pragma once

#include "Inputs.h"
#include "serialization/Serialization.h"
#include "reflection/Reflection.h"

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

		void Serialize(StreamWriter &ar) const;
		void Deserialize(StreamReader &ar);

		operator uint32_t() const { return (uint32_t)mKey; }

		REFLECTABLE()

	private:
		EKey mKey;
		bool mIsMouse;

		friend class FInputAction;
	};

	REFLECT(FInputKey)
	{
		BEGIN_REFLECT(FInputKey)
		REFLECT_PROPERTY("Key", GetKey, SetKey)
		REFLECT_PROPERTY_READ_ONLY("IsMouse", mIsMouse);
	}

} // namespace BHive
