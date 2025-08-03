#pragma once

#include "core/Core.h"
#include "world/ITickable.h"
#include "core/math/Transform.h"

namespace BHive
{
	class GameObject;

	struct BHIVE_API Component
	{

		virtual void Begin() {};

		virtual void Render() {}
		virtual void End() {};

		void SetOwner(GameObject *owner);

		GameObject *GetOwner() const { return mOwningObject; }
		FTransform GetWorldTransform() const;

		virtual void Save(cereal::BinaryOutputArchive &ar) const;
		virtual void Load(cereal::BinaryInputArchive &ar);

		virtual void Save(cereal::JSONOutputArchive &ar) const {};
		virtual void Load(cereal::JSONInputArchive &ar) {};

		REFLECTABLEV()

	private:
		GameObject *mOwningObject = nullptr;

		friend struct GameObject;
	};

	REFLECT(Component)
	{
		BEGIN_REFLECT(Component);
	}

} // namespace BHive