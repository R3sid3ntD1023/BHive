#pragma once

#include "core/Core.h"
#include "core/reflection/Reflection.h"
#include "core/serialization/Serialization.h"
#include "InputKey.h"

namespace BHive
{
	REFLECT_STRUCT()
	struct FInputAction
	{
		REFLECT_CONSTRUCTOR()
		FInputAction() = default;

		REFLECT_CONSTRUCTOR()
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
		REFLECT_PROPERTY()
		std::string mName;

		REFLECT_PROPERTY()
		FInputKey mKey;

		REFLECTABLE_CLASS()

		friend class InputContext;
	};

} // namespace BHive
