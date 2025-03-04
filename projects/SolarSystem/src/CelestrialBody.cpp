#include "CelestrialBody.h"
#include "components/IDComponent.h"
#include "components/TagComponent.h"

BEGIN_NAMESPACE(BHive)

CelestrialBody::CelestrialBody(Universe *universe)
	: mUniverse(universe)
{
	mIDComponent = AddComponent<IDComponent>();
	mTagComponent = AddComponent<TagComponent>();
}

void CelestrialBody::Begin()
{
	for (auto &component : mComponents)
	{
		component->Begin();
		if (component->IsTickEnabled())
		{
			mTickedComponents.push_back(component);
		}
	}
}

void CelestrialBody::Update(float dt)
{
	for (auto &component : mTickedComponents)
	{
		component->Update(dt);
	}
}

void CelestrialBody::SetName(const std::string &name)
{
	mTagComponent->Tag = name;
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

	ar.setNextName("Components");
	ar.startNode();
	ar(cereal::make_size_tag(mComponents.size()));

	for (auto &component : mComponents)
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
	ar(MAKE_NVP("Transform", mTransform), MAKE_NVP("Parent", mParent));

	ar.setNextName("Components");
	ar.startNode();
	ar(cereal::make_size_tag(num_components));

	mComponents.resize(num_components);
	for (size_t i = 0; i < num_components; i++)
	{
		rttr::type component_type = BHive::InvalidType;

		ar.startNode();
		ar(component_type);

		Ref<Component> component = nullptr;

		if (mComponents[i])
		{
			component = mComponents[i];
		}
		else
		{
			component = mComponents[i] = component_type.create().get_value<Ref<Component>>();
			component->SetOwner(this);
		}

		component->Load(ar);
		ar.finishNode();
	}

	ar.finishNode();
}

void CelestrialBody::AddComponent(const Ref<Component> &component)
{
	component->SetOwner(this);
	mComponents.push_back(component);
}

REFLECT(CelestrialBody)
{
	BEGIN_REFLECT(CelestrialBody)
	REFLECT_CONSTRUCTOR(Universe *);
}

END_NAMESPACE