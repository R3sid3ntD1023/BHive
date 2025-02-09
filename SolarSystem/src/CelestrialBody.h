#pragma once

#include <core/Core.h>
#include <core/reflection/Reflection.h>
#include <core/serialization/Serialization.h>
#include <math/Transform.h>

namespace BHive
{
	class Shader;
}

struct CelestrialBody
{
	void Update(const Ref<BHive::Shader> &shader, float);

	virtual void OnUpdate(const Ref<BHive::Shader> &shader, float) {}

	BHive::FTransform GetTransform() const;

	virtual void Save(cereal::JSONOutputArchive &ar) const;

	virtual void Load(cereal::JSONInputArchive &ar);

	REFLECTABLEV()

protected:
	BHive::FTransform mTransform;

private:
	CelestrialBody *mParent = nullptr;
	std::vector<Ref<CelestrialBody>> mChildren;
};

void Save(cereal::JSONOutputArchive &ar, const Ref<CelestrialBody> &obj);
void Load(cereal::JSONInputArchive &ar, Ref<CelestrialBody> &obj);

REFLECT(CelestrialBody)
{
	BEGIN_REFLECT(CelestrialBody);
}