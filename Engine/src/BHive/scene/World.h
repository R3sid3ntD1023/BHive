#pragma once

#include "asset/Asset.h"
#include "core/Core.h"
#include "core/UUID.h"
#include "math/Math.h"
#include "physics/PhysicsContext.h"
#include "physics/WorldEventListener.h"
#include "WorldPhysicsSettings.h"

namespace BHive
{
	class SceneRenderer;
	class Camera;
	class Entity;

	using entityPtr = Ref<Entity>;
	using Entities = std::unordered_map<UUID, entityPtr>;

	class BHIVE World : public Asset
	{
	public:
		World();
		~World();

		void UpdateEditor(Ref<SceneRenderer> renderer);

		void OnRuntimeStart();
		void OnRuntimeUpdate(float deltatime, Ref<SceneRenderer> renderer);
		void OnRuntimeStop();

		void OnSimulateStart();
		void OnSimulate(float deltatime, Ref<SceneRenderer> renderer);
		void OnSimulateStop();

		Ref<World> Copy() const;

		Ref<Entity> CreateEntity(const std::string &name = "NewEntity");

		void AddEntity(const Ref<Entity> &entity);

		Ref<Entity> DuplicateEntity(Entity *entity);

		void Step(int32_t frames = 1);

		bool IsPaused() const { return mIsPaused; }

		void SetPaused(bool pause);

		void OnViewportResize(uint32_t width, uint32_t height);

		rp3d::PhysicsWorld *GetPhysicsWorld() const { return mPhysicsWorld; }

		FWorldPhysicsSettings &GetWorldSettings() { return mPhysicsSettings; }

		Entities &GetEntities() { return mEntities; }

		const Entities &GetEntities() const { return mEntities; }

		struct CameraComponent *GetPrimaryCameraComponent() const;

		void RayCast(const glm::vec3 &start, const glm::vec3 &end, unsigned short mask = -1, float factor = 1.0f);

		virtual void Save(cereal::BinaryOutputArchive &ar) const;

		virtual void Load(cereal::BinaryInputArchive &ar);

		REFLECTABLEV(Asset)

	private:
		void OnPhysicsStart();
		void OnPhysicsUpdate(float deltatime);
		void OnPhysicsStop();

		void RenderScene(Ref<SceneRenderer> renderer);

		void OnCollisionContact(const rp3d::CollisionCallback::ContactPair &contact_pair);
		void OnCollisionOverlap(const rp3d::OverlapCallback::OverlapPair &overlap_pair);
		void OnHit(const rp3d::RaycastInfo &info);

		void OnEntityDestroyed(Entity *entity);

	private:
		bool mIsPaused = false;

		int32_t mFrames = 1;

		rp3d::PhysicsWorld *mPhysicsWorld = nullptr;

		FWorldPhysicsSettings mPhysicsSettings{};

		glm::uvec2 mViewportSize{0, 0};

		bool mInitialized = false;

		CollisionEventListener mCollisionListener;

		HitEventListener mHitListener;

		Entities mEntities;

		friend class SceneComponent;
	};

	REFLECT_EXTERN(World)

} // namespace BHive
