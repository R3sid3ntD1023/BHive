#pragma once

#include "Component.h"

BEGIN_NAMESPACE(BHive)

struct TagComponent : public Component
{
	TagComponent() { SetTickEnabled(false); }
	/* data */
	std::string Tag;

	virtual void Save(cereal::JSONOutputArchive &ar) const { ar(MAKE_NVP(Tag)); };

	virtual void Load(cereal::JSONInputArchive &ar) { ar(MAKE_NVP(Tag)); };

	REFLECTABLEV(Component)
};

END_NAMESPACE