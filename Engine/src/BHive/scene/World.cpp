#include "World.h"
#include "Actor.h"
#include "SceneRenderer.h"
#include "cameras/EditorCamera.h"
#include "debug/Instrumentor.h"
#include "core/Time.h"
#include "Components.h"

namespace BHive
{
	/*namespace utils
	{
		rp3d::Transform GetPhysicsTransform(const FTransform &t)
		{
			auto position = t.get_translation();
			auto rotation = glm::radians(t.get_rotation());

			auto quaternion = rp3d::Quaternion::fromEulerAngles(rp3d::Vector3(rotation.x, rotation.y, rotation.z));
			return rp3d::Transform({position.x, position.y, position.z}, quaternion);
		}

		glm::quat rp3d_to_quat(const rp3d::Quaternion &qua)
		{
			return glm::quat((float)qua.w, (float)qua.x, (float)qua.y, (float)qua.z);
		}

		FTransform GetTransform(const rp3d::Transform &t, const glm::vec3 &scale)
		{

			auto &position = t.getPosition();
			auto quat = rp3d_to_quat(t.getOrientation());
			glm::vec3 rotation = glm::degrees(glm::eulerAngles(quat));

			return FTransform({position.x, position.y, position.z}, {rotation.x, rotation.y, rotation.z}, scale);
		}

		rp3d::Vector3 LockAxisToVextor3(ELockAxis axis)
		{
			float x = (axis & ELockAxis::AxisX) != 0 ? 0.0f : 1.0f;
			float y = (axis & ELockAxis::AxisY) != 0 ? 0.0f : 1.0f;
			float z = (axis & ELockAxis::AxisZ) != 0 ? 0.0f : 1.0f;

			return {x, y, z};
		}
	}*/

	World::World()
	{
		mCollisionListener.OnContact.bind(this, &World::OnCollisionContact);
		mCollisionListener.OnTrigger.bind(this, &World::OnCollisionOverlap);
	}

	World::~World()
	{
		if (mPhysicsWorld)
		{
			PhysicsContext::get_context().destroyPhysicsWorld(mPhysicsWorld);
		}
	}

	void World::UpdateEditor(Ref<SceneRenderer> renderer)
	{
		RenderScene(renderer);
	}

	void World::OnRuntimeStart()
	{
		OnPhysicsStart();

		for (auto &[id, actor] : mActors)
		{
			actor->OnBegin();
		}

		mInitialized = true;
	}

	void World::OnRuntimeUpdate(float deltatime, Ref<SceneRenderer> renderer)
	{
		if (!mInitialized)
		{
			OnRuntimeStart();
		}

		CameraComponent *camera_component = GetPrimaryCameraComponent();

		if (camera_component)
		{
			const auto projection = camera_component->mCamera.GetProjection();
			const auto transform = camera_component->GetWorldTransform();

			renderer->Begin(projection, transform.inverse());

			RenderScene(renderer);

			renderer->End();
		}

		if (!mIsPaused || mFrames-- > 0)
		{
			OnPhysicsUpdate(deltatime);

			for (auto &[id, actor] : mActors)
			{
				actor->OnUpdate(deltatime);
			}
		}
	}

	void World::OnRuntimeStop()
	{
		OnPhysicsStop();

		for (auto &[id, actor] : mActors)
		{
			actor->OnEnd();
		}
	}

	void World::OnSimulateStart()
	{
		OnPhysicsStart();
	}

	void World::OnSimulate(float deltatime, Ref<SceneRenderer> renderer)
	{
		if (!mIsPaused || mFrames-- > 0)
		{
			OnPhysicsUpdate(deltatime);
		}

		RenderScene(renderer);
	}

	void World::OnSimulateStop()
	{
		OnPhysicsStop();
	}

	Ref<Actor> World::CreateActor(const std::string &name)
	{
		auto actor = CreateRef<Actor>();
		actor->SetName(name);
		AddActor(actor);

		return actor;
	}

	void World::AddActor(const Ref<Actor> &actor)
	{
		actor->mWorld = this;
		actor->OnActorDestroyed.bind(this, &World::OnActorDestroyed);
		mActors.emplace(actor->GetUUID(), actor);
	}

	Ref<Actor> World::DuplicateActor(Actor *actor)
	{
		if (!actor)
			return nullptr;

		auto duplicated = actor->get_type().create({*actor}).get_value<Ref<Actor>>();
		AddActor(duplicated);
		return duplicated;
	}

	void World::Step(int32_t frames)
	{
		mFrames = frames;
	}

	void World::SetPaused(bool pause)
	{
		mIsPaused = pause;
	}

	void World::OnViewportResize(uint32_t width, uint32_t height)
	{
		mViewportSize = {width, height};

		for (auto &[id, actor] : mActors)
		{
			for (auto &component : actor->GetComponents())
			{
				if (auto camera_component = Cast<CameraComponent>(component))
				{
					camera_component->mCamera.Resize(width, height);
				}
			}
		}
	}

	CameraComponent *World::GetPrimaryCameraComponent() const
	{
		for (auto &[id, actor] : mActors)
		{
			for (auto &component : actor->GetComponents())
			{
				if (auto camera_component = Cast<CameraComponent>(component))
				{
					if (camera_component->IsPrimary())
						return camera_component.get();
				}
			}
		}
		return nullptr;
	}

	void World::Serialize(StreamWriter &writer) const
	{
		writer(mActors.size());

		for (auto &[id, actor] : mActors)
		{
			writer(actor->get_type());

			actor->OnPreSave();
			actor->Serialize(writer);
		}
	}

	void World::Deserialize(StreamReader &reader)
	{
		size_t num_actors = 0;
		reader(num_actors);

		for (size_t i = 0; i < num_actors; i++)
		{
			AssetType actor_type = InvalidType;
			reader(actor_type);

			auto actor = actor_type.create().get_value<Ref<Actor>>();
			if (actor)
			{
				actor->Deserialize(reader);
				AddActor(actor);
			}
		}

		for (auto &[id, actor] : mActors)
			actor->OnPostLoad();
	}

	void World::OnPhysicsStart()
	{
		LOG_TRACE("Simulate Start");

		mPhysicsWorld = PhysicsContext::get_context().createPhysicsWorld(mPhysicsSettings);
		mPhysicsWorld->setEventListener(&mCollisionListener);

		// mPhysicsWorld->setGravity(rp3d::Vector3(0, -9.8, 0));
		mPhysicsWorld->setIsDebugRenderingEnabled(true);
		auto &debug_renderer = mPhysicsWorld->getDebugRenderer();
		debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_AABB, false);
		debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
		debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_NORMAL, false);
		debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_POINT, false);
		debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB, false);

		// auto view = mRegistry.view<RigidBodyComponent, TransformComponent>();
		// for (auto e : view)
		// {
		// 	auto [rbc, tc] = view.get(e);
		// 	Entity entity = {e, this};

		// 	auto uuid = entity.GetUUID();
		// 	auto &entity_id = mEntityMap.at(uuid);
		// 	auto transform = entity.GetTransform();

		// 	auto rb = mPhysicsWorld->createRigidBody(utils::GetPhysicsTransform(transform));
		// 	rb->setIsDebugEnabled(true);
		// 	rb->setUserData(&entity_id);
		// 	rb->setMass(rbc.mMass);
		// 	rb->setType((rp3d::BodyType)rbc.mBodyType);
		// 	rb->enableGravity(rbc.mGravityEnabled);
		// 	rb->setAngularDamping(rbc.mAngularDamping);
		// 	rb->setLinearDamping(rbc.mLinearDamping);

		// 	rb->setLinearLockAxisFactor(utils::LockAxisToVextor3(rbc.mLinearLockAxis));
		// 	rb->setAngularLockAxisFactor(utils::LockAxisToVextor3(rbc.mAngularLockAxis));

		// 	rp3d::Collider *collider = nullptr;
		// 	if (entity.HasComponent<BoxComponent>())
		// 	{
		// 		auto &boxcomponent = entity.GetComponent<BoxComponent>();
		// 		auto extents = boxcomponent.mExtents * 2.f * transform.get_scale();
		// 		auto offset = boxcomponent.mOffset * transform.get_scale();

		// 		auto offset_ = rp3d::Transform({offset.x, offset.y, offset.z},
		// 									   rp3d::Quaternion::identity());

		// 		auto boxshape = PhysicsContext::get_context().createBoxShape({extents.x * .5f, extents.y * .5f, extents.z * .5f});
		// 		collider = rb->addCollider(boxshape, offset_);
		// 		collider->setUserData(&entity_id);

		// 		collider->setIsTrigger(boxcomponent.mIsTrigger);

		// 		if (auto pm = boxcomponent.mPhysicsMaterial)
		// 		{
		// 			auto &material = collider->getMaterial();
		// 			material.setFrictionCoefficient(pm->mFrictionCoefficient);
		// 			material.setBounciness(pm->mBounciness);
		// 			material.setMassDensity(pm->mMassDensity);
		// 		}
		// 	}

		// 	if (entity.HasComponent<SphereComponent>())
		// 	{
		// 		auto &spherecomponent = entity.GetComponent<SphereComponent>();
		// 		auto extents = spherecomponent.mRadius * glm::compMax(transform.get_scale());
		// 		auto offset = spherecomponent.mOffset * transform.get_scale();

		// 		auto offset_ = rp3d::Transform({offset.x, offset.y, offset.z},
		// 									   rp3d::Quaternion::identity());

		// 		auto sphereshape = PhysicsContext::get_context().createSphereShape(extents);
		// 		collider = rb->addCollider(sphereshape, offset_);
		// 		collider->setUserData(&entity_id);

		// 		collider->setIsTrigger(spherecomponent.mIsTrigger);

		// 		if (auto pm = spherecomponent.mPhysicsMaterial)
		// 		{
		// 			auto &material = collider->getMaterial();
		// 			material.setFrictionCoefficient(pm->mFrictionCoefficient);
		// 			material.setBounciness(pm->mBounciness);
		// 			material.setMassDensity(pm->mMassDensity);
		// 		}
		// 	}

		// 	rbc.mRigidBodyInstance = (void *)rb;
		// }
	}

	void World::OnPhysicsUpdate(float deltatime)
	{
		BH_PROFILE_FUNCTION();

		const float timestep = 1.0f / 60.0f;
		static float accumulator = 0.0f;

		accumulator += deltatime;

		while (accumulator >= timestep)
		{
			mPhysicsWorld->update(timestep);
			accumulator -= timestep;
		}

		// // float factor = accumulator / timestep;

		// auto view = mRegistry.view<RigidBodyComponent, TransformComponent>();
		// for (auto e : view)
		// {
		// 	Entity entity{e, this};
		// 	auto [rbc, tc] = view.get(e);
		// 	auto rb = (rp3d::RigidBody *)rbc.mRigidBodyInstance;
		// 	if (!rb)
		// 		continue;

		// 	if (rbc.mBodyType == EBodyType::Dynamic)
		// 	{
		// 		auto &transform = rb->getTransform();
		// 		auto scale = entity.GetTransform().get_scale();
		// 		tc.mTransform = entity.GetParentTransform().inverse() * utils::GetTransform(transform, scale);
		// 	}
		// 	else
		// 	{
		// 		rb->setTransform(utils::GetPhysicsTransform(tc.mTransform));
		// 	}
		// }
	}

	void World::OnPhysicsStop()
	{
		LOG_TRACE("Simulate Stop");

		// auto view = mRegistry.view<RigidBodyComponent>();
		// for (auto e : view)
		// {
		// 	auto [rbc] = view.get(e);
		// 	mPhysicsWorld->destroyRigidBody((rp3d::RigidBody *)rbc.mRigidBodyInstance);
		// }

		PhysicsContext::get_context().destroyPhysicsWorld(mPhysicsWorld);
		mPhysicsWorld = nullptr;
	}

	void World::RenderScene(Ref<SceneRenderer> renderer)
	{
		BH_PROFILE_FUNCTION();

		for (auto &[id, actor] : mActors)
		{
			for (auto &component : actor->GetComponents())
			{
				if (auto renderable = Cast<IRenderable>(component))
				{
					renderable->OnRender(renderer.get());
				}
			}
		}
	}

	void World::OnCollisionContact(rp3d::CollisionCallback::ContactPair contact_pair)
	{
	}

	void World::OnCollisionOverlap(rp3d::OverlapCallback::OverlapPair overlap_pair)
	{
	}

	void World::OnActorDestroyed(Actor *actor)
	{
		if (mActors.contains(actor->GetUUID()))
			mActors.erase(actor->GetUUID());
	}
}
