#pragma once

#include "core/Core.h"
#include "core/UUID.h"
#include "asset/Asset.h"
#include "physics/PhysicsContext.h"
#include "physics/WorldEventListener.h"
#include "math/Math.h"
#include <entt/entt.hpp>

namespace BHive
{
	class SceneRenderer;
	class Camera;
	class Actor;

	using ActorPtr = Ref<Actor>;
	using ActorList = std::unordered_map<UUID, ActorPtr>;

	class BHIVE World : public Asset, public ISerializable
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

		Ref<Actor> CreateActor(const std::string &name = "NewActor");

		void AddActor(const Ref<Actor> &actor);

		Ref<Actor> DuplicateActor(Actor *actor);

		void Step(int32_t frames = 1);

		bool IsPaused() const { return mIsPaused; }

		void SetPaused(bool pause);

		void OnViewportResize(uint32_t width, uint32_t height);

		rp3d::PhysicsWorld *GetPhysicsWorld() const { return mPhysicsWorld; }

		rp3d::PhysicsWorld::WorldSettings &GetWorldSettings() { return mPhysicsSettings; }

		ActorList &GetActors() { return mActors; }

		const ActorList &GetActors() const { return mActors; }

		struct CameraComponent *GetPrimaryCameraComponent() const;

		void Serialize(StreamWriter &writer) const;
		void Deserialize(StreamReader &reader);

		ASSET_CLASS(World)

	private:
		void OnPhysicsStart();
		void OnPhysicsUpdate(float deltatime);
		void OnPhysicsStop();

		void RenderScene(Ref<SceneRenderer> renderer);

		void OnCollisionContact(rp3d::CollisionCallback::ContactPair contact_pair);
		void OnCollisionOverlap(rp3d::OverlapCallback::OverlapPair overlap_pair);

		void OnActorDestroyed(Actor *actor);

		entt::registry &GetRegistry()
		{
			return mEnttRegistry;
		}

	private:
		bool mIsPaused = false;

		int32_t mFrames = 1;

		rp3d::PhysicsWorld *mPhysicsWorld = nullptr;

		rp3d::PhysicsWorld::WorldSettings mPhysicsSettings{};

		glm::uvec2 mViewportSize{0, 0};

		bool mInitialized = false;

		WorldEventListener mCollisionListener;

		ActorList mActors;

		entt::registry mEnttRegistry;

		friend class SceneComponent;
	};

}
