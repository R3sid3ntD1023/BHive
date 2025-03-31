#pragma once

#include "core/Core.h"
#include "core/reflection/Reflection.h"
#include "core/serialization/Serialization.h"
#include "InputKey.h"

namespace BHive
{
	DECLARE_STRUCT()
	struct FInputAction
	{
		DECLARE_CONSTRUCTOR()
		FInputAction() = default;

		DECLARE_CONSTRUCTOR()
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
		DECLARE_PROPERTY()
		std::string mName;

		DECLARE_PROPERTY()
		FInputKey mKey;

		REFLECTABLE_CLASS()

		friend class InputContext;
	};

} // namespace BHive
