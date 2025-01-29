#include "scene/Entity.h"
#include "SceneComponent.h"

namespace BHive
{
	/*void SceneComponent::SetWorldTransform(const FTransform &transform)
	{
		mLocalTransform = GetOwner()->GetWorldTransform().inverse() * transform;
	}*/

	FTransform SceneComponent::GetWorldTransform() const
	{
		return GetOwner()->GetWorldTransform() /** mLocalTransform*/;
	}

	/*void SceneComponent::SetLocalTransform(const FTransform &transform)
	{
		mLocalTransform = transform;
	}

	const FTransform &SceneComponent::GetLocalTransform() const
	{
		return mLocalTransform;
	}*/

	void SceneComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		Component::Save(ar);
		// ar(mLocalTransform);
	}

	void SceneComponent::Load(cereal::BinaryInputArchive &ar)
	{
		Component::Load(ar);
		// ar(mLocalTransform);
	}

	REFLECT(SceneComponent)
	{
		BEGIN_REFLECT(SceneComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS();
	}

} // namespace BHive