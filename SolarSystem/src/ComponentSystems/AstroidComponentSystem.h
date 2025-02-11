#pragma once

#include <core/UUID.h>
#include "ComponentSystem.h"

namespace BHive
{
	class Shader;
	class VertexBuffer;
} // namespace BHive

struct AstroidComponentSystem : public ComponentSystem
{
	AstroidComponentSystem();

	/* data */
	virtual void Update(class Universe *universe, float dt) override;

private:
	Ref<BHive::Shader> mInstanceShader;
	std::unordered_map<BHive::UUID, Ref<BHive::VertexBuffer>> mVertexBuffers;
};
