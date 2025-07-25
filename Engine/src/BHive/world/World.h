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

	using ObjectList = std::unordered_map<UUID, Ref<GameObject>>;
	using EnTTList = std::unordered_map<entt::entity, UUID>;

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

		Ref<GameObject> CreateGameObject(const rttr::type &type);

		void AddGameObject(const Ref<GameObject> &object);

		Ref<GameObject> GetGameObject(const UUID &id) const;

		const ObjectList &GetGameObjects() const { return mObjects; }

		void Destroy(const UUID &id);

		bool RayCast(
			const glm::vec3 &start, const glm::vec3 &dir, float maxDistance, FHitResult &result,
			uint16_t categoryMasks = 65535U);

		bool IsRunning() const { return mIsRunning; }
		bool IsPaused() const { return mIsPaused; }

		std::pair<Camera *, FTransform> GetPrimaryCamera();

		REFLECTABLEV(Asset);

	protected:
		ObjectList mObjects;
		EnTTList mEnttMap;

		std::vector<Ref<GameObject>> mDestoryedObjects;

		// entt
		entt::registry mRegistry;

#pragma region Physics
		float mAccumulatedTime{0.0f};
		bool mIsRunning = false;
		bool mIsPaused = false;
		int32_t mFrames = 1;
		physx::PxScene *mPhyxWorld = nullptr;
		physx::PxDefaultCpuDispatcher *mCpuDispatcher = nullptr;
		physx::PxSimulationEventCallback *mSimulationEventCallback = nullptr;
		physx::PxRaycastCallback *mHitCallback = nullptr;
#pragma endregion

		friend class GameObject;
	};

	REFLECT_EXTERN(World)
} // namespace BHive