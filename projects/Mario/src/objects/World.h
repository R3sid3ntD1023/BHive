#pragma once

#include "core/Core.h"
#include "physics/EventListener.h"
#include "physics/PhysicsContext.h"
#include "asset/Asset.h"
#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace BHive
{
	class GameObject;
	class Texture2D;
	using ObjectList = std::unordered_map<UUID, Ref<GameObject>>;

	class World : public Asset
	{
	public:
		World();
		World(const World &world);
		~World();

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		void Begin();
		void Update(float dt);
		void Render();
		void End();

		void SimulateBegin();
		void Simulate(float dt);
		void SimulateEnd();
		void SetPaused(bool paused);
		Ref<World> Copy() const;

		void RenderPhysicsWorld();
		void Resize(uint32_t w, uint32_t h);

		template <typename T = GameObject>
		Ref<T> CreateGameObject(const std::string &name)
			requires(std::is_base_of_v<GameObject, T>)
		{
			auto object = CreateRef<T>(mRegistry.create(), this);
			object->SetName(name);
			AddGameObject(object);
			return object;
		}

		void AddGameObject(const Ref<GameObject> &object);

		Ref<GameObject> GetGameObject(const UUID &id) const;

		entt::entity CreateEntity();

		const ObjectList &GetGameObjects() const { return mObjects; }

		void Destroy(const UUID &id);

		void RayCast(const glm::vec3 &start, const glm::vec3 &end, uint16_t categoryMasks = 65535U);

		rp3d::PhysicsWorld *GetPhysicsWorld() const { return mPhysicsWorld; }

		bool IsRunning() const { return mIsRunning; }
		bool IsPaused() const { return mIsPaused; }

		REFLECTABLEV(Asset);

	private:
		ObjectList mObjects;
		std::vector<Ref<GameObject>> mDestoryedObjects;

		// physics
		rp3d::PhysicsWorld *mPhysicsWorld = nullptr;
		float mAccumulatedTime{0.0f};
		CollisionEventListener mCollisionListener;
		HitEventListener mHitListener;

		bool mIsRunning = false;
		bool mIsPaused = false;

		entt::registry mRegistry;

		friend class GameObject;
	};
} // namespace BHive