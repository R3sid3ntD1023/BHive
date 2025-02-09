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
	Planet();

	virtual void OnUpdate(const Ref<BHive::Shader> &shader, float dt) override;

	void Initialize(const PlanetData &data);

	void Save(cereal::JSONOutputArchive &ar) const override;

	void Load(cereal::JSONInputArchive &ar) override;

	REFLECTABLEV(CelestrialBody)

private:
	PlanetData mData;

	Ref<BHive::Texture> mTexture;
	static inline Ref<BHive::StaticMesh> mSphere;
};

REFLECT(Planet)
{
	BEGIN_REFLECT(Planet)
	REFLECT_CONSTRUCTOR();
}