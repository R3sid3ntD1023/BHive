#include "CelestrialBody.h"
#include "Universe.h"

void Universe::Update(const Ref<BHive::Shader> &shader, float dt)
{
	for (auto &body : mBodies)
	{
		body->Update(shader, dt);
	}
}

void Universe::Save(cereal::JSONOutputArchive &ar) const
{
	ar(mBodies);
}

void Universe::Load(cereal::JSONInputArchive &ar)
{

	ar(mBodies);
}
