#pragma once

#include "ComponentSystem.h"

namespace BHive
{
	class Shader;
	class VertexBuffer;
} // namespace BHive

struct RenderSystem : public ComponentSystem
{
	RenderSystem();

	virtual void Update(class Universe *universe, float dt) override;

private:
	Ref<BHive::Shader> mShader;
	Ref<BHive::Shader> mInstanceShader;
	std::unordered_map<BHive::UUID, Ref<BHive::VertexBuffer>> mVertexBuffers;
};
