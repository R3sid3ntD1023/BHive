#pragma once

#include "core/Core.h"
#include "core/reflection/Reflection.h"
#include "core/serialization/Serialization.h"
#include "core/math/Transform.h"

namespace BHive
{
	struct GameObject;

	struct Component
	{

		virtual void Begin() {};
		virtual void Update(float) {};
		virtual void Render() {}
		virtual void End() {};

		void SetTickEnabled(bool enabled);
		void SetOwner(GameObject *owner);

		bool IsTickEnabled() const { return mTickEnabled; }
		GameObject *GetOwner() const { return mOwningObject; }
		FTransform GetWorldTransform() const;

		virtual void Save(cereal::BinaryOutputArchive &ar) const;
		virtual void Load(cereal::BinaryInputArchive &ar);

		virtual void Save(cereal::JSONOutputArchive &ar) const {};
		virtual void Load(cereal::JSONInputArchive &ar) {};

		REFLECTABLEV()

	private:
		GameObject *mOwningObject = nullptr;
		bool mTickEnabled = true;
		friend struct GameObject;
	};

	REFLECT(Component)
	{
		BEGIN_REFLECT(Component) REFLECT_PROPERTY("TickEnabled", mTickEnabled);
	}

} // namespace BHive