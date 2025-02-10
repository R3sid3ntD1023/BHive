#include "CelestrialBody.h"
#include "Universe.h"

void Universe::Update(const Ref<BHive::Shader> &shader, float dt)
{
	for (auto &[id, body] : mBodies)
	{
		body->Update(shader, dt);
	}
}

void Universe::Save(cereal::JSONOutputArchive &ar) const
{
	ar(cereal::make_size_tag(mBodies.size()));
	for (auto &[id, body] : mBodies)
	{
		ar.startNode();

		ar(MAKE_NVP("ID", id));
		ar(MAKE_NVP("Type", body->get_type()));
		body->Save(ar);

		ar.finishNode();
	}
}

void Universe::Load(cereal::JSONInputArchive &ar)
{
	size_t size = 0;
	ar(cereal::make_size_tag(size));

	mBodies.reserve(size);

	for (size_t i = 0; i < size; i++)
	{
		ar.startNode();

		BHive::UUID id = 0;
		rttr::type type = BHive::InvalidType;

		ar(MAKE_NVP("ID", id));
		ar(MAKE_NVP("Type", type));

		if (type)
		{
			auto body = type.create({mRegistry.create(), this}).get_value<Ref<CelestrialBody>>();
			body->Load(ar);

			mBodies.emplace(id, body);
		}

		ar.finishNode();
	}
}

Ref<CelestrialBody> Universe::GetBody(const BHive::UUID &id) const
{
	return mBodies.at(id);
}
