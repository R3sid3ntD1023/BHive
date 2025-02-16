#pragma once

#include "ComponentSystem.h"
#include <entt/entt.hpp>

namespace BHive
{
	class Shader;
	class StorageBuffer;
	class BindlessTexture;
} // namespace BHive

struct RenderSystem : public ComponentSystem
{
	RenderSystem();

	virtual void Update(class Universe *universe, float dt) override;
	void SetPreZPass(bool enabled);
	bool IsPreZPassEnabled() const { return mPreZPass; }

private:
	void ZPrePass(class Universe *universe);
	void Render(class Universe *universe);

private:
	Ref<BHive::Shader> mShader;
	Ref<BHive::Shader> mZPrePassShader;
	std::unordered_map<BHive::UUID, Ref<BHive::StorageBuffer>> mStorageBuffer;
	// std::unordered_map<BHive::UUID, Ref<BHive::BindlessTexture>> mTextures;
	bool mPreZPass = true;
};
