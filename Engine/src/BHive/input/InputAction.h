#pragma once

#include "core/Core.h"
#include "core/reflection/Reflection.h"
#include "core/serialization/Serialization.h"
#include "InputKey.h"

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

		REFLECTABLEV()

		friend class InputContext;
	};

} // namespace BHive
