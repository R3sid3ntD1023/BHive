#include "CelestrialBody.h"
#include "Component.h"
#include <core/serialization/Serialization.h>

BEGIN_NAMESPACE(BHive)

CelestrialBody::CelestrialBody(World *world)
	: GameObject(world)
{
}

void CelestrialBody::Save(cereal::JSONOutputArchive &ar) const
{
	auto &components = GetComponents();
	ar(MAKE_NVP("ID", mID), MAKE_NVP("Name", mName), MAKE_NVP("Transform", mTransform), MAKE_NVP("Parent", mParent));

	ar.setNextName("Components");
	ar.startNode();
	ar(cereal::make_size_tag(components.size()));

	for (auto &component : components)
	{
		ar.startNode();
		ar(component->get_type());
		component->Save(ar);
		ar.finishNode();
	}

	ar.finishNode();
}

void CelestrialBody::Load(cereal::JSONInputArchive &ar)
{
	size_t num_components = 0;
	ar(MAKE_NVP("ID", mID), MAKE_NVP("Name", mName), MAKE_NVP("Transform", mTransform), MAKE_NVP("Parent", mParent));

	ar.setNextName("Components");
	ar.startNode();
	ar(cereal::make_size_tag(num_components));

	auto &components = GetComponents();
	components.resize(num_components);
	for (size_t i = 0; i < num_components; i++)
	{
		rttr::type component_type = BHive::InvalidType;

		ar.startNode();
		ar(component_type);

		Ref<Component> component = nullptr;

		auto it = std::find_if(
			components.begin(), components.end(),
			[&](const Ref<Component> &comp) { return comp && comp->get_type() == component_type; });

		if (it != components.end())
		{
			component = *it;
		}
		else
		{
			component = components[i] = component_type.create().get_value<Ref<Component>>();
			component->SetOwner(this);
		}

		component->Load(ar);
		ar.finishNode();
	}

	ar.finishNode();
}

REFLECT(CelestrialBody)
{
	BEGIN_REFLECT(CelestrialBody)
	REFLECT_CONSTRUCTOR(Universe *);
}

END_NAMESPACE