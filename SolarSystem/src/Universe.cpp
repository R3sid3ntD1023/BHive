#include "Universe.h"
#include "CelestrialBody.h"
#include "ComponentSystems/StarComponentSystem.h"
#include "ComponentSystems/PlanetComponentSystem.h"
#include "ComponentSystems/MeshComponentSystem.h"
#include "ComponentSystems/AstroidComponentSystem.h"

Universe::Universe()
{
	mSystems.push_back(CreateRef<PlanetComponentSystem>());
	mSystems.push_back(CreateRef<StarComponentSystem>());
	mSystems.push_back(CreateRef<MeshComponentSystem>());
	mSystems.push_back(CreateRef<AstroidComponentSystem>());
}

void Universe::AddBody(const Ref<CelestrialBody> &body)
{
	mBodies.emplace(BHive::UUID(), body);
}

void Universe::Update(float dt)
{
	for (auto &system : mSystems)
	{
		system->Update(this, dt);
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
	if (mBodies.contains(id))
		return mBodies.at(id);

	return nullptr;
}
