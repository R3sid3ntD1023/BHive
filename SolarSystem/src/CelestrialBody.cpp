#include "CelestrialBody.h"
#include "Universe.h"

CelestrialBody::CelestrialBody(const entt::entity &entity, Universe *universe)
	: mEntityHandle(entity),
	  mUniverse(universe)
{
}

void CelestrialBody::Update(const Ref<BHive::Shader> &shader, float dt)
{
	OnUpdate(shader, dt);
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
	ar(MAKE_NVP("Transform", mTransform), MAKE_NVP("Components", mComponents), MAKE_NVP("Parent", mParent));
}

void CelestrialBody::Load(cereal::JSONInputArchive &ar)
{
	ar(MAKE_NVP("Transform", mTransform), MAKE_NVP("Components", mComponents), MAKE_NVP("Parent", mParent));
}

REFLECT(CelestrialBody)
{
	BEGIN_REFLECT(CelestrialBody)
	REFLECT_CONSTRUCTOR(const entt::entity &, Universe *);
}