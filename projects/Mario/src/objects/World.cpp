#include "components/Components.h"
#include "components/InputComponent.h"
#include "components/BoxComponent.h"
#include "components/PhysicsComponent.h"
#include "GameObject.h"
#include "gfx/RenderCommand.h"
#include "renderers/Renderer.h"
#include "World.h"
#include "physics/PhysicsUtils.h"

namespace BHive
{
	World::World(const std::string &name)
		: mName(name)
	{
		mCollisionListener.OnContact.bind(
			[](const rp3d::CollisionCallback::ContactPair &p)
			{
				auto bd1 = (GameObject *)p.getBody1()->getUserData();
				auto bd2 = (GameObject *)p.getBody2()->getUserData();
				auto c1 = (ColliderComponent *)p.getCollider1()->getUserData();
				auto c2 = (ColliderComponent *)p.getCollider2()->getUserData();
				auto event = p.getEventType();

				switch (event)
				{
				case reactphysics3d::CollisionCallback::ContactPair::EventType::ContactStart:
					c1->OnCollisionEnter.invoke(c2, bd2);
					c2->OnCollisionEnter.invoke(c1, bd1);
					break;
				case reactphysics3d::CollisionCallback::ContactPair::EventType::ContactStay:
					c1->OnCollisionStay.invoke(c2, bd2);
					c2->OnCollisionStay.invoke(c1, bd1);
					break;
				case reactphysics3d::CollisionCallback::ContactPair::EventType::ContactExit:
					c1->OnCollisionExit.invoke(c2, bd2);
					c2->OnCollisionExit.invoke(c1, bd1);
					break;
				default:
					break;
				}
			});

		mCollisionListener.OnTrigger.bind(
			[](const rp3d::OverlapCallback::OverlapPair &p)
			{
				auto bd1 = (GameObject *)p.getBody1()->getUserData();
				auto bd2 = (GameObject *)p.getBody2()->getUserData();
				auto c1 = (ColliderComponent *)p.getCollider1()->getUserData();
				auto c2 = (ColliderComponent *)p.getCollider2()->getUserData();
				auto event = p.getEventType();

				switch (event)
				{
				case reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapStart:
					c1->OnTriggerEnter.invoke(c2, bd2);
					c2->OnTriggerEnter.invoke(c1, bd1);
					break;
				case reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapStay:
					c1->OnTriggerStay.invoke(c2, bd2);
					c2->OnTriggerStay.invoke(c1, bd1);
					break;
				case reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapExit:
					c1->OnTriggerExit.invoke(c2, bd2);
					c2->OnTriggerExit.invoke(c1, bd1);
					break;
				default:
					break;
				}
			});

		mHitListener.OnHit.bind(
			[](const rp3d::RaycastInfo &hit)
			{
				auto body = (GameObject *)hit.body->getUserData();
				auto collider = (ColliderComponent *)hit.collider->getUserData();

				if (collider)
				{
					auto p = hit.worldPoint;
					auto n = hit.worldNormal;
					collider->OnRaycastHit({p.x, p.y, p.z}, {n.x, n.y, n.z}, hit.hitFraction);
				}
			});
	}

	World::~World()
	{
	}

	void World::Begin()
	{
		SimulateBegin();

		auto inputs = mRegistry.view<InputComponent>();
		inputs.each([](InputComponent &input) { input.CreateInstance(); });
	}

	void World::Update(float dt)
	{
		auto inputs = mRegistry.view<InputComponent>();
		inputs.each(
			[](InputComponent &input)
			{
				if (auto context = input.GetContext())
				{
					context->process();
				}
			});

		Simulate(dt);
		Render();

		for (auto &[entity_id, id] : mDestroyedObjects)
		{
			mRegistry.erase(entity_id);
			mObjects.erase(id);
		}
	}

	void World::End()
	{
		auto inputs = mRegistry.view<InputComponent>();
		inputs.each([](InputComponent &input) { input.DestroyInstance(); });

		SimulateEnd();
	}

	void World::SimulateBegin()
	{
		rp3d::PhysicsWorld::WorldSettings settings{};
		settings.worldName = mName;
		mPhysicsWorld = PhysicsContext::get_context().createPhysicsWorld(settings);

		// setup listeners

		mPhysicsWorld->setEventListener(&mCollisionListener);

#ifdef _DEBUG
		mPhysicsWorld->setIsDebugRenderingEnabled(true);
		auto &debug_renderer = mPhysicsWorld->getDebugRenderer();
		debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);
		debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
		debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);
		debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_POINT, true);
		debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB, true);
#endif

		auto view = mRegistry.view<IDComponent, PhysicsComponent>();
		view.each(
			[&](IDComponent &id, PhysicsComponent &physics)
			{
				auto object = mObjects.at(id.ID);
				auto t = object->GetTransform();
				auto rb = mPhysicsWorld->createRigidBody(physics::utils::GetPhysicsTransform(t));
				rb->setIsDebugEnabled(true);
				rb->setUserData(object.get());
				rb->setMass(physics.mMass);
				rb->setType((rp3d::BodyType)physics.mBodyType);
				rb->enableGravity(physics.mGravityEnabled);
				rb->setAngularDamping(physics.mAngularDamping);
				rb->setLinearDamping(physics.mLinearDamping);

				rb->setLinearLockAxisFactor(physics::utils::LockAxisToVextor3(physics.mLinearLockAxis));
				rb->setAngularLockAxisFactor(physics::utils::LockAxisToVextor3(physics.mAngularLockAxis));

				physics.SetRigidBody(rb);

				if (object->HasComponent<BoxComponent>())
				{
					auto &box = object->GetComponent<BoxComponent>();
					box.CreateCollsionShape(rb, t);
				}
			});
	}

	void World::Simulate(float dt)
	{
		float time_step = 1.f / 60.f;

		mAccumulatedTime += dt;

		while (mAccumulatedTime >= time_step)
		{
			mPhysicsWorld->update(time_step);
			mAccumulatedTime -= time_step;
		}

		auto view = mRegistry.view<TransformComponent, PhysicsComponent>();
		view.each(
			[](TransformComponent &t, PhysicsComponent &p)
			{
				auto rb = Cast<rp3d::RigidBody>(p.GetRigidBody());

				if (!rb)
					return;

				switch (p.mBodyType)
				{
				case EBodyType::Dynamic:
				{
					auto &transform = rb->getTransform();
					auto scale = t.Transform.get_scale();
					t.Transform = physics::utils::GetTransform(transform, scale);
					break;
				}
				case EBodyType::Kinematic:
				{
					rb->setTransform(physics::utils::GetPhysicsTransform(t.Transform));
					break;
				}
				default:
					break;
				}
			});
	}

	void World::SimulateEnd()
	{
		auto view = mRegistry.view<IDComponent, PhysicsComponent>();
		view.each(
			[&](IDComponent &id, PhysicsComponent &physics)
			{
				auto rb = Cast<rp3d::RigidBody>(physics.GetRigidBody());
				auto object = mObjects.at(id.ID);

				mPhysicsWorld->destroyRigidBody(rb);
				physics.SetRigidBody(nullptr);
			});

		PhysicsContext::get_context().destroyPhysicsWorld(mPhysicsWorld);
		mPhysicsWorld = nullptr;
	}

	void World::Render()
	{
		RenderCommand::Clear();

		Camera *camera = nullptr;
		FTransform transform;

		{
			auto view = mRegistry.view<CameraComponent, IDComponent>();
			view.each(
				[&](CameraComponent &c, IDComponent &id)
				{
					if (c.Primary)
					{
						auto object = mObjects.at(id.ID);
						camera = &c.Camera;
						transform = object->GetTransform();
					}
				});
		}

		if (camera)
		{
			Renderer::SubmitCamera(camera->GetProjection(), transform.inverse());

			Renderer::Begin();

			{
				auto view = mRegistry.view<SpriteComponent, IDComponent>();
				view.each(
					[&](SpriteComponent &s, IDComponent &id)
					{
						auto object = mObjects.at(id.ID);
						auto t = object->GetTransform();
						QuadRenderer::DrawSprite({s.SpriteSize, s.Tiling, s.SpriteColor}, s.Sprite, t);
					});
			}

#ifdef _DEBUG
			RenderPhysicsWorld();
#endif // DEBUG

			Renderer::End();
		}
	}

	void World::RenderPhysicsWorld()
	{
		if (mPhysicsWorld->getIsDebugRenderingEnabled())
		{
			auto &physics_debugger = mPhysicsWorld->getDebugRenderer();

			auto &lines = physics_debugger.getLines();
			auto &tris = physics_debugger.getTriangles();

			for (unsigned i = 0; i < lines.size(); i++)
			{
				auto &line = lines[i];
				Line debug_line;
				debug_line.color = line.color1;
				debug_line.p0 = {line.point1.x, line.point1.y, line.point1.z};
				debug_line.p1 = {line.point2.x, line.point2.y, line.point2.z};
				LineRenderer::DrawLine(debug_line, {});
			}

			for (unsigned i = 0; i < tris.size(); i++)
			{
				auto &tri = tris[i];
				glm::vec3 p0 = {tri.point1.x, tri.point1.y, tri.point1.z};
				glm::vec3 p1 = {tri.point2.x, tri.point2.y, tri.point2.z};
				glm::vec3 p2 = {tri.point3.x, tri.point3.y, tri.point3.z};
				LineRenderer::DrawTriangle(p0, p1, p2, tri.color2, {});
			}
		}
	}

	void World::Resize(uint32_t w, uint32_t h)
	{
		auto view = mRegistry.view<CameraComponent>();
		view.each([w, h](CameraComponent &component) { component.Camera.Resize(w, h); });

		RenderCommand::SetViewport(0, 0, w, h);
	}

	void World::AddGameObject(const Ref<GameObject> &object)
	{
		mObjects.emplace(object->GetID(), object);
	}

	Ref<GameObject> World::GetGameObject(const UUID &id) const
	{
		return mObjects.at(id);
	}

	void World::Destroy(const UUID &id)
	{
		if (!mObjects.contains(id))
			return;

		auto object = mObjects.at(id);
		mDestroyedObjects.emplace(*object, id);
	}

	void World::RayCast(const glm::vec3 &start, const glm::vec3 &end, uint16_t categoryMasks)
	{
		rp3d::Ray ray({start.x, start.y, start.z}, {end.x, end.y, end.z});
		mPhysicsWorld->raycast(ray, &mHitListener, categoryMasks);
	}
} // namespace BHive