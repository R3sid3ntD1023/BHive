#pragma once

#include "ComponentSystem.h"
#include "core/UUID.h"

namespace BHive
{
	class Shader;
	class StorageBuffer;
} // namespace BHive

struct CullingSystem : public ComponentSystem
{
	CullingSystem();

	void Update(Universe *universe, float dt) override;

	std::unordered_map<BHive::UUID, Ref<BHive::StorageBuffer>> mInstanced;
	Ref<BHive::Shader> mCullingShader;
};