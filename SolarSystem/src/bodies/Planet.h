#pragma once

#include "CelestrialBody.h"
#include "PlanetData.h"

namespace BHive
{
	class StaticMesh;
	class Texture;

} // namespace BHive

struct Planet : public CelestrialBody
{
	Planet(const entt::entity &entity, Universe *universe);

	virtual void OnUpdate(const Ref<BHive::Shader> &shader, float dt) override;

	void Save(cereal::JSONOutputArchive &ar) const override;

	void Load(cereal::JSONInputArchive &ar) override;

	REFLECTABLEV(CelestrialBody)

private:
	PlanetData mData;
};
