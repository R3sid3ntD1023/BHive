#pragma once

#include "core/Core.h"
#include "core/reflection/Reflection.h"
#include "core/serialization/Serialization.h"

namespace BHive
{
	struct GameObject;

	DECLARE_CLASS()
	struct Component
	{

		virtual void Begin() {};
		virtual void Update(float) {};
		virtual void Render() {}
		virtual void End() {};

		virtual void Save(cereal::BinaryOutputArchive &ar) const {};
		virtual void Load(cereal::BinaryInputArchive &ar) {};

		GameObject *GetOwner() const { return mOwningObject; }

		REFLECTABLE_CLASS()

	private:
		GameObject *mOwningObject = nullptr;
		friend struct GameObject;
	};

} // namespace BHive