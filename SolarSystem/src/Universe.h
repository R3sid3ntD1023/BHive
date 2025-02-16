#pragma once

#include <core/Core.h>
#include <core/serialization/Serialization.h>
#include <core/UUID.h>
#include <entt/entt.hpp>

struct CelestrialBody;

namespace BHive
{
	class Shader;
	class VertexBuffer;
	class StaticMesh;
} // namespace BHive

class Universe
{
public:
	Universe();

	template <typename T>
	Ref<CelestrialBody> AddBody()
	{
		auto body = CreateRef<T>(mRegistry.create(), this);
		AddBody(body);
		return body;
	}

	void AddBody(const Ref<CelestrialBody> &body);

	void Update(float dt);

	void Save(cereal::JSONOutputArchive &ar) const;
	void Load(cereal::JSONInputArchive &ar);

	Ref<CelestrialBody> GetBody(const BHive::UUID &id) const;

	entt::registry &GetRegistry() { return mRegistry; }

	Ref<struct ComponentSystem> &GetRenderSystem() { return mRenderSystem; }

private:
	std::unordered_map<BHive::UUID, Ref<CelestrialBody>> mBodies;
	entt::registry mRegistry;
	Ref<struct ComponentSystem> mRenderSystem;
	Ref<struct ComponentSystem> mPlanetSystem;
};
