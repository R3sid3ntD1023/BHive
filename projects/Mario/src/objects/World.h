#pragma once

#include "core/Core.h"
#include "physics/EventListener.h"
#include "physics/PhysicsContext.h"
#include <glm/glm.hpp>

namespace BHive
{
	class GameObject;
	class Texture2D;
	using ObjectList = std::unordered_map<UUID, Ref<GameObject>>;

	class World
	{
	public:
		World(const std::string &name);
		~World();

		void Begin();
		void Update(float dt);
		void Render();
		void End();

		void SimulateBegin();
		void Simulate(float dt);
		void SimulateEnd();

		void RenderPhysicsWorld();
		void Resize(uint32_t w, uint32_t h);

		template <typename T = GameObject>
		Ref<T> CreateGameObject(const std::string &name)
			requires(std::is_base_of_v<GameObject, T>)
		{
			auto object = CreateRef<T>(this);
			object->SetName(name);
			AddGameObject(object);
			return object;
		}

		void AddGameObject(const Ref<GameObject> &object);

		Ref<GameObject> GetGameObject(const UUID &id) const;

		const ObjectList &GetGameObjects() const { return mObjects; }

		const std::string &GetName() const { return mName; }

		void Destroy(const UUID &id);

		void RayCast(const glm::vec3 &start, const glm::vec3 &end, uint16_t categoryMasks = 65535U);

		rp3d::PhysicsWorld *GetPhysicsWorld() const { return mPhysicsWorld; }

	private:
		std::string mName;
		ObjectList mObjects;
		std::vector<Ref<GameObject>> mDestoryedObjects;

		// physics
		rp3d::PhysicsWorld *mPhysicsWorld = nullptr;
		float mAccumulatedTime{0.0f};
		CollisionEventListener mCollisionListener;
		HitEventListener mHitListener;

		bool mIsRunning = false;

		friend class GameObject;
	};
} // namespace BHive