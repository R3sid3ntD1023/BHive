#include "InstancedStaticMeshComponent.h"
#include "runtime/GameObject.h"

namespace BHive
{
	void InstancedStaticMeshComponent::SetInstances(const std::vector<FTransform> &transforms)
	{
		mInstances.resize(transforms.size());

		for (size_t i = 0; i < transforms.size(); i++)
			mInstances.emplace_back(transforms[i]);
	}

	void InstancedStaticMeshComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		StaticMeshComponent::Save(ar);
		ar(mInstances);
	}

	void InstancedStaticMeshComponent::Load(cereal::BinaryInputArchive &ar)
	{
		StaticMeshComponent::Load(ar);
		ar(mInstances);
	}

	REFLECT(InstancedStaticMeshComponent)
	{
		BEGIN_REFLECT(InstancedStaticMeshComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() COMPONENT_IMPL();
	}
} // namespace BHive