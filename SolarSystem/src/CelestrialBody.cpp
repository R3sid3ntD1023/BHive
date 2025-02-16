#include "CelestrialBody.h"
#include "components/IDComponent.h"
#include "components/TagComponent.h"

CelestrialBody::CelestrialBody(const entt::entity &entity, Universe *universe)
	: mEntityHandle(entity),
	  mUniverse(universe)
{
	AddComponent<IDComponent>();
	AddComponent<TagComponent>();
}

void CelestrialBody::SetParent(const BHive::UUID &parent)
{
	mParent = parent;
}

BHive::FTransform CelestrialBody::GetTransform() const
{

	if (!mParent)
	{
		return mTransform;
	}

	auto parent = mUniverse->GetBody(mParent);
	return parent->GetTransform() * mTransform;
}

void CelestrialBody::Save(cereal::JSONOutputArchive &ar) const
{
	ar(MAKE_NVP("Transform", mTransform), MAKE_NVP("Parent", mParent));

	std::vector<rttr::type> components;
	auto component_types = rttr::type::get<Component>().get_derived_classes();
	for (auto &type : component_types)
	{
		if (type.get_method(HAS_COMPONENT_FUNC_NAME).invoke(*this))
		{
			components.push_back(type);
		}
	}

	ar.setNextName("Components");
	ar.startNode();
	ar(cereal::make_size_tag(components.size()));

	for (auto &type : components)
	{
		auto *component = &type.get_method(GET_COMPONENT_FUNC_NAME).invoke(*this).get_value<Component &>();

		ar.startNode();
		ar(type);
		component->Save(ar);
		ar.finishNode();
	}

	ar.finishNode();
}

void CelestrialBody::Load(cereal::JSONInputArchive &ar)
{
	size_t num_components = 0;
	ar(MAKE_NVP("Transform", mTransform), MAKE_NVP("Parent", mParent));

	ar.setNextName("Components");
	ar.startNode();
	ar(cereal::make_size_tag(num_components));

	for (size_t i = 0; i < num_components; i++)
	{
		rttr::type component_type = BHive::InvalidType;

		ar.startNode();
		ar(component_type);

		Component *component = nullptr;

		if (component_type.get_method(HAS_COMPONENT_FUNC_NAME).invoke(*this).to_bool())
		{
			component = component_type.get_method(GET_COMPONENT_FUNC_NAME).invoke(*this).get_value<Component *>();
		}
		else
		{
			component = component_type.get_method(ADD_COMPONENT_FUNC_NAME).invoke(*this).get_value<Component *>();
		}

		component->Load(ar);
		ar.finishNode();
	}

	ar.finishNode();
}

REFLECT(CelestrialBody)
{
	BEGIN_REFLECT(CelestrialBody)
	REFLECT_CONSTRUCTOR(const entt::entity &, Universe *);
}