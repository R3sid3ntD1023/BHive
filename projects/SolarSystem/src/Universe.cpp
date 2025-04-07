#include "CelestrialBody.h"
#include "renderer/RenderPipeline.h"
#include "Universe.h"

BEGIN_NAMESPACE(BHive)

Universe::Universe()
{
}

void Universe::Save(cereal::JSONOutputArchive &ar) const
{
	ar(cereal::make_size_tag(mObjects.size()));
	for (auto &[id, body] : mObjects)
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

	mObjects.reserve(size);

	for (size_t i = 0; i < size; i++)
	{
		ar.startNode();

		rttr::type type = BHive::InvalidType;

		ar(MAKE_NVP("Type", type));

		if (type)
		{
			auto body = type.create({this}).get_value<Ref<CelestrialBody>>();
			body->Load(ar);

			mObjects.emplace(body->GetID(), body);
		}

		ar.finishNode();
	}
}

END_NAMESPACE