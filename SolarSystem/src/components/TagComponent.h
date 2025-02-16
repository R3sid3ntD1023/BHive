#pragma once

#include "Component.h"

struct TagComponent : public Component
{
	/* data */
	std::string Tag;

	virtual void Save(cereal::JSONOutputArchive &ar) const { ar(MAKE_NVP(Tag)); };

	virtual void Load(cereal::JSONInputArchive &ar) { ar(MAKE_NVP(Tag)); };

	REFLECTABLEV(Component)
};
