#pragma once

#include <core/Core.h>
#include <core/serialization/Serialization.h>
#include <core/UUID.h>

struct CelestrialBody;

namespace BHive
{
	class Shader;
}

class Universe
{
public:
	void Update(const Ref<BHive::Shader> &shader, float dt);

	void Save(cereal::JSONOutputArchive &ar) const;
	void Load(cereal::JSONInputArchive &ar);

private:
	std::vector<Ref<CelestrialBody>> mBodies;
};
