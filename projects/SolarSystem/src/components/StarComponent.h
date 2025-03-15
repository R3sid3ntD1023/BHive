#pragma once

#include "Component.h"
#include "renderers/Lights.h"
#include <gfx/Color.h>

BEGIN_NAMESPACE(BHive)

struct StarComponent : public Component
{
	float Brightness{1.0f};
	float Radius{1.0f};
	FColor Color;

	StarComponent() = default;

	virtual void Update(float dt) override;

	virtual void Save(cereal::JSONOutputArchive &ar) const override;

	virtual void Load(cereal::JSONInputArchive &ar) override;

	REFLECTABLEV(Component)

private:
	PointLight mLight;
};

END_NAMESPACE