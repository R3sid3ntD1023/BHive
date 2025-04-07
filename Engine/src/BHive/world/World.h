#pragma once

#include "asset/Asset.h"
#include "core/Core.h"
#include "math/Transform.h"
#include "physics/EventListener.h"
#include "physics/PhysicsContext.h"
#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace BHive
{
	class GameObject;
	class Texture2D;
	class Camera;

	using ObjectList = std::unordered_map<UUID, Ref<GameObject>>;
	using EnTTList = std::unordered_map<entt::entity, UUID>;

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
		void Step(int32_t frames = 1);

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

		const ObjectList &GetGameObjects() const { return mObjects; }

		void Destroy(const UUID &id);

		void RayCast(const glm::vec3 &start, const glm::vec3 &end, uint16_t categoryMasks = 65535U);

		rp3d::PhysicsWorld *GetPhysicsWorld() const { return mPhysicsWorld; }

		bool IsRunning() const { return mIsRunning; }
		bool IsPaused() const { return mIsPaused; }

		std::pair<Camera *, FTransform> GetPrimaryCamera();

		REFLECTABLEV(Asset);

	protected:
		ObjectList mObjects;
		EnTTList mEnttMap;

		std::vector<Ref<GameObject>> mDestoryedObjects;

		// physics
		rp3d::PhysicsWorld *mPhysicsWorld = nullptr;
		float mAccumulatedTime{0.0f};
		CollisionEventListener mCollisionListener;
		HitEventListener mHitListener;

		bool mIsRunning = false;
		bool mIsPaused = false;
		int32_t mFrames = 1;

		// entt
		entt::registry mRegistry;

		friend class GameObject;
	};
} // namespace BHive