#pragma once

#include "Component.h"
#include <gfx/Color.h>

struct StarComponent : public Component
{
	float mBrightness{1.0f};
	float mRadius{1.0f};
	BHive::Color mColor;

	virtual void Save(cereal::JSONOutputArchive &ar) const override;

	virtual void Load(cereal::JSONInputArchive &ar) override;

	REFLECTABLEV(Component)
};