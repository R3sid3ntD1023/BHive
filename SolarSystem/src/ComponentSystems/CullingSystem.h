#pragma once

#include "ComponentSystem.h"
#include "core/UUID.h"

namespace BHive
{
	class Shader;
	class VertexBuffer;
} // namespace BHive

struct CullingSystem : public ComponentSystem
{
	CullingSystem();

	void Update(Universe *universe, float dt) override;

	std::unordered_map<BHive::UUID, Ref<BHive::VertexBuffer>> mInstanced;
	Ref<BHive::Shader> mCullingShader;
	bool First = true;

	uint32_t mTransformFeedback = 0;
};