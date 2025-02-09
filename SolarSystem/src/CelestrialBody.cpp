#include "CelestrialBody.h"

void CelestrialBody::Update(const Ref<BHive::Shader> &shader, float dt)
{
	OnUpdate(shader, dt);

	for (auto &child : mChildren)
	{
		child->Update(shader, dt);
	}
}

BHive::FTransform CelestrialBody::GetTransform() const
{
	if (!mParent)
		return mTransform;

	return mParent->GetTransform() * mTransform;
}

void CelestrialBody::Save(cereal::JSONOutputArchive &ar) const
{
	ar(mTransform, mChildren);
}

void CelestrialBody::Load(cereal::JSONInputArchive &ar)
{
	ar(mTransform, mChildren);

	for (auto &child : mChildren)
		child->mParent = this;
}

void Save(cereal::JSONOutputArchive &ar, const Ref<CelestrialBody> &obj)
{
	ar(obj->get_type());
	obj->Save(ar);
}

void Load(cereal::JSONInputArchive &ar, Ref<CelestrialBody> &obj)
{
	rttr::type type = BHive::InvalidType;
	ar(type);
	obj = type.create().get_value<Ref<CelestrialBody>>();
	obj->Load(ar);
}
