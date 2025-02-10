#pragma once

#include "CelestrialBody.h"
#include <gfx/Color.h>

namespace BHive
{
	class StaticMesh;

} // namespace BHive

class Star : public CelestrialBody
{
public:
	float Brightness = 1.f;
	float Radius = 20.f;
	BHive::Color Color = {1.f, 1.f, 1.f, 1.f};
	BHive::Color Emission = {10.f, 5.f, .5f, 1.f};

public:
	Star(const entt::entity &entity, Universe *universe);

	virtual void OnUpdate(const Ref<BHive::Shader> &shader, float);

	void Save(cereal::JSONOutputArchive &ar) const;

	void Load(cereal::JSONInputArchive &ar);

	REFLECTABLEV(CelestrialBody)
};
