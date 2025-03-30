#include "components//IDComponent.h"
#include "components/BoxComponent.h"
#include "components/CameraComponent.h"
#include "components/Component.h"
#include "components/InputComponent.h"
#include "components/PhysicsComponent.h"
#include "components/SpriteComponent.h"
#include "GameObject.h"
#include "gfx/RenderCommand.h"
#include "physics/PhysicsUtils.h"
#include "renderers/Renderer.h"
#include "World.h"

namespace BHive
{
	void CopyComponents(const GameObject &src, GameObject &dst)
	{
		for (auto &component : src.GetComponents())
		{
			auto type = component->get_type();
			type.get_method(EMPLACE_OR_REPLACE_COMPONENT_FUNCTION_NAME).invoke(dst, *component);
		}
	}

	World::World()
		: Asset()
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

	World::World(const World &world)
		: Asset(world)
	{
		mCollisionListener = world.mCollisionListener;
		mHitListener = world.mHitListener;
	}

	World::~World()
	{
	}

	entt::entity World::CreateEntity()
	{
		return mRegistry.create();
	}

	void World::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);

		ar(mObjects.size());
		for (auto &[id, obj] : mObjects)
		{
			ar(obj ? true : false);
			if (obj)
			{
				ar(obj->get_type());
				obj->Save(ar);
			}
		}
	}

	void World::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);

		size_t num_objects = 0;
		ar(num_objects);

		for (size_t i = 0; i < num_objects; i++)
		{
			bool is_valid = false;
			rttr::type obj_type = InvalidType;

			ar(is_valid);

			if (!is_valid)
				continue;

			ar(obj_type);
			auto obj = obj_type.create({mRegistry.create(), this}).get_value<Ref<GameObject>>();
			obj->Load(ar);

			AddGameObject(obj);
		}
	}

	void World::Begin()
	{
		SimulateBegin();

		for (auto &object : mObjects)
			object.second->Begin();

		mIsRunning = true;
	}

	void World::Update(float dt)
	{
		if (mIsRunning)
		{
			if (!mIsPaused || mFrames-- > 0)
			{
				Simulate(dt);

				for (auto &object : mObjects)
					object.second->Update(dt);
			}
		}

		Render();

		for (auto &object : mDestoryedObjects)
		{
			if (mIsRunning)
				object->End();

			mObjects.erase(object->GetID());
			mRegistry.destroy((entt::entity)*object);
		}

		mDestoryedObjects.clear();
	}

	void World::Render()
	{

		for (auto &object : mObjects)
			object.second->Render();

#ifdef _DEBUG
		RenderPhysicsWorld();
#endif // DEBUG
	}

	void World::End()
	{
		for (auto &object : mObjects)
			object.second->End();

		SimulateEnd();

		mIsRunning = false;
	}

	void World::SimulateBegin()
	{
		rp3d::PhysicsWorld::WorldSettings settings{};
		settings.worldName = GetName();
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
	}

	void World::SimulateEnd()
	{
		PhysicsContext::get_context().destroyPhysicsWorld(mPhysicsWorld);
		mPhysicsWorld = nullptr;
	}

	void World::SetPaused(bool paused)
	{
		mIsPaused = paused;
	}

	void World::Step(int32_t frames)
	{
		mFrames = frames;
	}

	Ref<World> World::Copy() const
	{
		auto new_world = CreateRef<World>(*this);
		new_world->SetName("Instance");

		auto &dst_registry = new_world->mRegistry;

		auto &objects = GetGameObjects();

		for (auto &[id, src_obj] : objects)
		{
			auto type = src_obj->get_type();

			auto new_obj = type.create({dst_registry.create(), new_world.get()}).get_value<Ref<GameObject>>();

			if (!new_obj)
				continue;

			CopyComponents(*src_obj, *new_obj);
			new_world->AddGameObject(new_obj);
		}

		return new_world;
	}

	void World::RenderPhysicsWorld()
	{
		if (!mPhysicsWorld)
			return;

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
		for (auto &object : mObjects)
		{
			if (object.second->HasComponent<CameraComponent>())
			{
				auto &camera = object.second->GetComponent<CameraComponent>()->Camera;
				camera.Resize(w, h);
			}
		}

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

		mDestoryedObjects.push_back(mObjects.at(id));
	}

	void World::RayCast(const glm::vec3 &start, const glm::vec3 &end, uint16_t categoryMasks)
	{
		rp3d::Ray ray({start.x, start.y, start.z}, {end.x, end.y, end.z});
		mPhysicsWorld->raycast(ray, &mHitListener, categoryMasks);
	}

	std::pair<Camera *, FTransform> World::GetPrimaryCamera()
	{
		auto view = mRegistry.view<CameraComponent, IDComponent>();
		for (auto &e : view)
		{
			auto [camera_component, id_component] = view.get(e);
			if (camera_component.IsPrimary)
			{
				Camera *camera = &camera_component.Camera;
				auto t = GetGameObject(id_component.ID)->GetTransform().inverse();
				return std::make_pair(camera, t);
			}
		}
		return {nullptr, FTransform()};
	}
} // namespace BHive