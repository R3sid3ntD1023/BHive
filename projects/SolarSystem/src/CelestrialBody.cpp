#include "CelestrialBody.h"
#include "Component.h"
#include <core/serialization/Serialization.h>

BEGIN_NAMESPACE(BHive)

CelestrialBody::CelestrialBody(const entt::entity &handle, World *world)
	: GameObject(handle, world)
{
}

void CelestrialBody::Save(cereal::JSONOutputArchive &ar) const
{
	GameObject::Save(ar);

	auto &components = GetComponents();

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
	GameObject::Load(ar);

	size_t num_components = 0;

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

		Component *component = nullptr;

		if (component_type.get_method(HAS_COMPONENT_FUNCTION_NAME).invoke({this}).to_bool())
		{
			component = component_type.get_method(GET_COMPONENT_FUNCTION_NAME).invoke({this}).get_value<Component *>();
		}
		else
		{
			component = component_type.get_method(ADD_COMPONENT_FUNCTION_NAME).invoke({this}).get_value<Component *>();
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