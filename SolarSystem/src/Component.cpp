#include "Component.h"

void Save(cereal::JSONOutputArchive &ar, const Ref<Component> &obj)
{
	ar(MAKE_NVP("Type", obj->get_type()));
	obj->Save(ar);
}

void Load(cereal::JSONInputArchive &ar, Ref<Component> &obj)
{
	rttr::type type = BHive::InvalidType;
	ar(MAKE_NVP("Type", type));
	obj = type.create().get_value<Ref<Component>>();
	obj->Load(ar);
}