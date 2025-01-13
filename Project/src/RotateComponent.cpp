#include "RotateComponent.h"
#include "scene/Entity.h"

namespace BHive
{
    void RotateComponent::OnBegin()
    {
   
    }

    void RotateComponent::OnUpdate(float dt)
    {
        auto entity = this->GetOwner();

        auto transform = entity->GetLocalTransform();
        transform.add_rotation({0, mRotateSpeed * dt, 0});

        entity->SetLocalTransform(transform);
	}

	void RotateComponent::Save(cereal::JSONOutputArchive &ar) const
	{
		Component::Save(ar);
		ar(MAKE_NVP("RotationSpeed", mRotateSpeed));
	}

	void RotateComponent::Load(cereal::JSONInputArchive &ar)
	{
		Component::Load(ar);
		ar(MAKE_NVP("RotationSpeed", mRotateSpeed));
	}


    REFLECT(RotateComponent)
    {
        BEGIN_REFLECT(RotateComponent)
        (META_DATA(ClassMetaData_ComponentSpawnable, true))
            REQUIRED_COMPONENT_FUNCS()
                REFLECT_PROPERTY("RotateSpeed", mRotateSpeed);
    }
}