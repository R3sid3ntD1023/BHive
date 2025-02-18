#pragma once

#include "Component.h"
#include <gfx/Color.h>

struct StarComponent : public Component
{
	float Brightness{1.0f};
	float Radius{1.0f};
	BHive::Color Color;

	StarComponent() = default;

	virtual void Update(float dt) override;

	virtual void Save(cereal::JSONOutputArchive &ar) const override;

	virtual void Load(cereal::JSONInputArchive &ar) override;

	REFLECTABLEV(Component)
};