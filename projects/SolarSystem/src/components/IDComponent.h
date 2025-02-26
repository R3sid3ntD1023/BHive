#pragma once

#include <core/UUID.h>
#include "Component.h"

struct IDComponent : public Component
{
	IDComponent() { SetTickEnabled(false); }

	BHive::UUID mID;

	virtual void Save(cereal::JSONOutputArchive &ar) const override;

	virtual void Load(cereal::JSONInputArchive &ar) override;

	REFLECTABLEV(Component);
};
