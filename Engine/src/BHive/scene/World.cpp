#include "cameras/EditorCamera.h"
#include "Components.h"
#include "core/Time.h"
#include "debug/Instrumentor.h"
#include "Entity.h"
#include "SceneRenderer.h"
#include "serialization/Serialization.h"
#include "World.h"

namespace BHive
{
	World::World()
	{
		mCollisionListener.OnContact.bind(this, &World::OnCollisionContact);
		mCollisionListener.OnTrigger.bind(this, &World::OnCollisionOverlap);
		mHitListener.OnHit.bind(this, &World::OnHit);
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

		for (auto &[id, entity] : mEntities)
		{
			entity->OnBegin();
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

			for (auto &[id, entity] : mEntities)
			{
				entity->OnUpdate(deltatime);
			}
		}
	}

	void World::OnRuntimeStop()
	{
		for (auto &[id, entity] : mEntities)
		{
			entity->OnEnd();
		}

		OnPhysicsStop();
	}

	void World::OnSimulateStart()
	{
		OnPhysicsStart();

		for (auto &[id, entity] : mEntities)
		{
			entity->OnBegin();
		}
	}

	void World::OnSimulate(float deltatime, Ref<SceneRenderer> renderer)
	{
		if (!mIsPaused || mFrames-- > 0)
		{
			OnPhysicsUpdate(deltatime);

			for (auto &[id, entity] : mEntities)
			{
				entity->OnUpdate(deltatime);
			}
		}

		RenderScene(renderer);
	}

	void World::OnSimulateStop()
	{
		for (auto &[id, entity] : mEntities)
		{
			entity->OnEnd();
		}

		OnPhysicsStop();
	}

	Ref<World> World::Copy() const
	{
		auto new_world = CreateRef<World>(*this);
		new_world->mEntities.clear();

		for (const auto &[id, entity] : mEntities)
		{
			auto new_entity = entity->Copy();
			new_world->AddEntity(new_entity);
		}

		return new_world;
	}

	Ref<Entity> World::CreateEntity(const std::string &name)
	{
		auto entity = CreateRef<Entity>();
		entity->SetName(name);
		AddEntity(entity);

		return entity;
	}

	void World::AddEntity(const Ref<Entity> &entity)
	{
		entity->mWorld = this;
		entity->OnEntityDestroyed.bind(this, &World::OnEntityDestroyed);
		mEntities.emplace(entity->GetUUID(), entity);
	}

	Ref<Entity> World::DuplicateEntity(Entity *entity)
	{
		if (!entity)
			return nullptr;

		auto duplicated = entity->Duplicate(true);
		AddEntity(duplicated);
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

		for (auto &[id, entity] : mEntities)
		{
			for (auto &component : entity->GetComponents())
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
		for (auto &[id, entity] : mEntities)
		{
			for (auto &component : entity->GetComponents())
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

	void World::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mEntities.size());

		for (auto &[id, entity] : mEntities)
		{
			auto type = entity->get_type();

			ar(type);

			entity->Save(ar);
		}
	}

	void World::Load(cereal::BinaryInputArchive &ar)
	{
		size_t num_entitys = 0;
		ar(num_entitys);

		for (size_t i = 0; i < num_entitys; i++)
		{
			AssetType entity_type = InvalidType;
			ar(entity_type);

			auto entity = entity_type.create().get_value<Ref<Entity>>();
			if (entity)
			{
				entity->Load(ar);
				AddEntity(entity);
			}
		}
	}

	void World::OnPhysicsStart()
	{
		LOG_TRACE("Simulate Start");

		mPhysicsWorld = PhysicsContext::get_context().createPhysicsWorld(mPhysicsSettings);
		mPhysicsWorld->setEventListener(&mCollisionListener);

		// mPhysicsWorld->setGravity(rp3d::Vector3(0, -9.8, 0));
		mPhysicsWorld->setIsDebugRenderingEnabled(true);
		auto &debug_renderer = mPhysicsWorld->getDebugRenderer();
		debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);
		debug_renderer.setIsDebugItemDisplayed(
			rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
		debug_renderer.setIsDebugItemDisplayed(
			rp3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);
		debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_POINT, true);
		debug_renderer.setIsDebugItemDisplayed(
			rp3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB, true);
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
	}

	void World::OnPhysicsStop()
	{
		LOG_TRACE("Simulate Stop");

		PhysicsContext::get_context().destroyPhysicsWorld(mPhysicsWorld);
		mPhysicsWorld = nullptr;
	}

	void World::RenderScene(Ref<SceneRenderer> renderer)
	{
		for (auto &[id, entity] : mEntities)
		{
			for (auto &component : entity->GetComponents())
			{
				if (auto renderable = Cast<IRenderable>(component))
				{
					renderable->OnRender(renderer.get());
				}
			}
		}
	}

	void
	World::RayCast(const glm::vec3 &start, const glm::vec3 &end, unsigned short mask, float factor)
	{
		rp3d::Ray ray({start.x, start.y, start.z}, {end.x, end.y, end.z}, factor);
		mPhysicsWorld->raycast(ray, &mHitListener, mask);
	}

	void World::OnCollisionContact(const rp3d::CollisionCallback::ContactPair &contact_pair)
	{
		auto bd1 = (Entity *)contact_pair.getBody1()->getUserData();
		auto bd2 = (Entity *)contact_pair.getBody2()->getUserData();
		auto c1 = (ColliderComponent *)contact_pair.getCollider1()->getUserData();
		auto c2 = (ColliderComponent *)contact_pair.getCollider2()->getUserData();
		auto event = contact_pair.getEventType();

		switch (event)
		{
		case reactphysics3d::CollisionCallback::ContactPair::EventType::ContactStart:
			c1->OnCollisionEnter.invoke(c2, bd1, bd2);
			c2->OnCollisionEnter.invoke(c1, bd1, bd2);
			break;
		case reactphysics3d::CollisionCallback::ContactPair::EventType::ContactStay:
			c1->OnCollisionStay.invoke(c2, bd1, bd2);
			c2->OnCollisionStay.invoke(c1, bd1, bd2);
			break;
		case reactphysics3d::CollisionCallback::ContactPair::EventType::ContactExit:
			c1->OnCollisionExit.invoke(c2, bd1, bd2);
			c2->OnCollisionExit.invoke(c1, bd1, bd2);
			break;
		default:
			break;
		}
	}

	void World::OnCollisionOverlap(const rp3d::OverlapCallback::OverlapPair &overlap_pair)
	{
		auto bd1 = (Entity *)overlap_pair.getBody1()->getUserData();
		auto bd2 = (Entity *)overlap_pair.getBody2()->getUserData();
		auto c1 = (ColliderComponent *)overlap_pair.getCollider1()->getUserData();
		auto c2 = (ColliderComponent *)overlap_pair.getCollider2()->getUserData();
		auto event = overlap_pair.getEventType();

		switch (event)
		{
		case reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapStart:
			c1->OnTriggerEnter.invoke(c2, bd1, bd2);
			c2->OnTriggerEnter.invoke(c1, bd1, bd2);
			break;
		case reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapStay:
			c1->OnTriggerStay.invoke(c2, bd1, bd2);
			c2->OnTriggerStay.invoke(c1, bd1, bd2);
			break;
		case reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapExit:
			c1->OnTriggerExit.invoke(c2, bd1, bd2);
			c2->OnTriggerExit.invoke(c1, bd1, bd2);
			break;
		default:
			break;
		}
	}

	void World::OnHit(const rp3d::RaycastInfo &info)
	{
		auto body = (Entity *)info.body->getUserData();
		auto collider = (ColliderComponent *)info.collider->getUserData();

		if (collider)
		{
			auto p = info.worldPoint;
			auto n = info.worldNormal;
			collider->OnRaycastHit({p.x, p.y, p.z}, {n.x, n.y, n.z}, info.hitFraction);
		}
	}

	void World::OnEntityDestroyed(Entity *entity)
	{
		if (mEntities.contains(entity->GetUUID()))
			mEntities.erase(entity->GetUUID());
	}

} // namespace BHive
