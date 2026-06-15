#pragma once

#include "core/Core.h"
#include "runtime/ITickable.h"
#include "core/math/Transform.h"

namespace BHive
{
	class GameObject;

	struct BHIVE_API Component
	{
		virtual ~Component() = default;

		virtual void Begin() {};

		virtual void End() {};

		virtual void Save(cereal::BinaryOutputArchive &ar) const;

		virtual void Load(cereal::BinaryInputArchive &ar);

		void SetOwner(GameObject *owner);

		GameObject *GetOwner() const { return mOwningObject; }

		FTransform GetWorldTransform() const;

		REFLECTABLEV()

	private:
		GameObject *mOwningObject = nullptr;

		friend GameObject;
	};

	REFLECT_INLINE(Component)
	{
		BEGIN_REFLECT(Component);
	}

} // namespace BHive