#pragma once

#include <core/UUID.h>
#include "Component.h"

BEGIN_NAMESPACE(BHive)

struct IDComponent : public Component
{
	IDComponent() { SetTickEnabled(false); }

	UUID mID;

	virtual void Save(cereal::JSONOutputArchive &ar) const override;

	virtual void Load(cereal::JSONInputArchive &ar) override;

	REFLECTABLEV(Component);
};

END_NAMESPACE