#pragma once

#include "core/Core.h"
#include "asset/Asset.h"
#include "core/math/Transform.h"
#include "physics/EventListener.h"
#include "physics/PhysicsContext.h"
#include <entt/entt.hpp>

namespace physx
{
	class PxScene;
	class PxDefaultCpuDispatcher;
	class PxSimulationEventCallback;
} // namespace physx

namespace BHive
{
	class GameObject;
	class Texture2D;
	class Camera;
	class SceneRenderer;

	using ObjectIdentifier = std::pair<UUID, entt::entity>;
	using ObjectList = std::unordered_map<UUID, Ref<GameObject>>;
	using EnTTList = std::unordered_map<entt::entity, UUID>;
	using PendingDestroyQueue = std::queue<ObjectIdentifier>;

	class BHIVE_API World : public Asset
	{
	public:
		World();
		World(const World &world);
		~World();

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		virtual void Save(cereal::JSONOutputArchive &ar) const;
		virtual void Load(cereal::JSONInputArchive &ar);

		void Begin();
		void Update(float dt, SceneRenderer *renderer);
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
			auto object = CreateRef<T>(name, mRegistry.create(), this);
			AddGameObject(object);
			return object;
		}

		Ref<GameObject> CreateGameObject(const rttr::type &type);

		void AddGameObject(const Ref<GameObject> &object);

		GameObject *DuplicateGameobject(GameObject *object);

		Ref<GameObject> GetGameObject(const UUID &id) const;

		const ObjectList &GetGameObjects() const { return mObjects; }

		void Destroy(const UUID &gameObjectID);

		bool RayCast(const glm::vec3 &start, const glm::vec3 &dir, float maxDistance, FHitResult &result, uint16_t categoryMasks = 65535U);

		bool IsRunning() const { return mIsRunning; }

		bool IsPaused() const { return mIsPaused; }

		std::pair<Camera *, FTransform> GetPrimaryCamera();

		const entt::registry &GetRegistry() const { return mRegistry; }

		REFLECTABLEV(Asset);

	protected:
		entt::registry mRegistry;
		ObjectList mObjects;
		EnTTList mEnttMap;
		PendingDestroyQueue mDestroyQueue;

		bool mIsRunning = false;
		bool mIsPaused = false;

#pragma region Physics
		float mAccumulatedTime{0.0f};

		int32_t mFrames = 1;
		physx::PxScene *mPhyxWorld = nullptr;
		physx::PxDefaultCpuDispatcher *mCpuDispatcher = nullptr;
		physx::PxSimulationEventCallback *mSimulationEventCallback = nullptr;
		physx::PxRaycastCallback *mHitCallback = nullptr;
#pragma endregion

#pragma region SYSTEMS
		Ref<struct RenderSystem> mRenderSystem;
#pragma endregion

		friend class GameObject;
	};

	REFLECT_EXTERN(World)
} // namespace BHive