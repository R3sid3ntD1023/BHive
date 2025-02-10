#pragma once

#include "ComponentSystem.h"

namespace BHive
{
	class Shader;
}

struct MeshComponentSystem : public ComponentSystem
{
	MeshComponentSystem();

	virtual void Update(class Universe *universe, float dt) override;

private:
	Ref<BHive::Shader> mShader;
};
