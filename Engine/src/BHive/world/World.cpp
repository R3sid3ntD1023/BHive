#include "Component.h"
#include "components/BoxComponent.h"
#include "components/CameraComponent.h"
#include "components/FlipBookComponent.h"
#include "components/InputComponent.h"
#include "components/PhysicsComponent.h"
#include "components/SpriteComponent.h"
#include "GameObject.h"
#include "gfx/RenderCommand.h"
#include "physics/EventListener.h"
#include "physics/PhysicsUtils.h"
#include "renderers/Renderer.h"
#include "World.h"
#include <physx/PxPhysicsAPI.h>

namespace BHive
{
	namespace callbacks
	{

	} // namespace callbacks

	void CopyComponents(const GameObject &src, GameObject &dst)
	{
		dst.GetComponents().clear();
		for (auto &component : src.GetComponents())
		{
			auto type = component->get_type();
			type.get_method(EMPLACE_OR_REPLACE_COMPONENT_FUNCTION_NAME).invoke(dst, component);
		}
	}

	World::World()
		: Asset()
	{
		/*mCollisionListener.OnContact.bind(
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
			});*/
	}

	World::World(const World &world)
		: Asset(world)
	{
	}

	World::~World()
	{
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

			if (!obj_type)
				continue;

			auto obj = obj_type.create({mRegistry.create(), this}).get_value<Ref<GameObject>>();
			obj->Load(ar);

			AddGameObject(obj);
		}
	}
	void World::Save(cereal::JSONOutputArchive &ar) const
	{
		ar(cereal::make_size_tag(mObjects.size()));
		for (auto &[id, body] : mObjects)
		{
			ar.startNode();
			ar(MAKE_NVP("Type", body->get_type()));
			body->Save(ar);

			ar.finishNode();
		}
	}

	void World::Load(cereal::JSONInputArchive &ar)
	{
		size_t size = 0;
		ar(cereal::make_size_tag(size));

		mObjects.reserve(size);

		for (size_t i = 0; i < size; i++)
		{
			ar.startNode();

			rttr::type type = BHive::InvalidType;

			ar(MAKE_NVP("Type", type));

			if (type)
			{
				auto body = type.create({mRegistry.create(), this}).get_value<Ref<GameObject>>();
				body->Load(ar);

				mObjects.emplace(body->GetID(), body);
			}

			ar.finishNode();
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

		// physx
		auto physics = PhysicsContext::GetPhysics();
		physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

		// init callbacks
		mSimulationEventCallback = new SimulationCallback();

		mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = mCpuDispatcher;
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		mPhyxWorld = physics->createScene(sceneDesc);
		mPhyxWorld->setSimulationEventCallback(mSimulationEventCallback);

#ifdef _DEBUG
		mPhyxWorld->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
		mPhyxWorld->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 1.0f);

		mPhyxWorld->setVisualizationParameter(physx::PxVisualizationParameter::eBODY_AXES, 1.0f);
		mPhyxWorld->setVisualizationParameter(physx::PxVisualizationParameter::eBODY_MASS_AXES, 1.0f);
		mPhyxWorld->setVisualizationParameter(physx::PxVisualizationParameter::eBODY_ANG_VELOCITY, 1.0f);
		mPhyxWorld->setVisualizationParameter(physx::PxVisualizationParameter::eBODY_LIN_VELOCITY, 1.0f);

		mPhyxWorld->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_DYNAMIC, 1.0f);
		mPhyxWorld->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_AABBS, 1.0f);
		mPhyxWorld->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_COMPOUNDS, 1.0f);
		mPhyxWorld->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_EDGES, 1.0f);
#endif // _DEBUG

		physx::PxPvdSceneClient *pvdClient = mPhyxWorld->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}

		auto view = mRegistry.view<PhysicsComponent>();
		for (auto &e : view)
		{
			auto [physics_component] = view.get(e);
			auto &settings = physics_component.Settings;

			if (!settings.PhysicsEnabled)
				continue;

			auto gameobject = mObjects[mEnttMap[e]];
			auto t = gameobject->GetTransform();

			physx::PxRigidActor *rigid_body = nullptr;
			switch (settings.BodyType)
			{
			case EBodyType::Static:
			{
				auto body = physics->createRigidStatic(physics::utils::getTransform(t));
				rigid_body = body;
			}
			break;
			case EBodyType::Dynamic:
			case EBodyType::Kinematic:
			{
				auto body = physics->createRigidDynamic(physics::utils::getTransform(t));
				body->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, settings.BodyType == EBodyType::Kinematic);
				body->setAngularDamping(settings.AngularDamping);
				body->setLinearDamping(settings.LinearDamping);
				body->setMass(settings.Mass);
				body->setRigidDynamicLockFlags(
					physics::utils::GetLockFlags(settings.LinearLockAxis, settings.AngularLockAxis));
				body->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, settings.GravityEnabled);
				body->userData = gameobject.get();
				rigid_body = body;
			}
			break;
			default:
				break;
			}

			mPhyxWorld->addActor(*rigid_body);
			physics_component.SetRigidBody(rigid_body);

			/*auto rb = mPhysicsWorld->createRigidBody(physics::utils::GetPhysicsTransform(t));
			rb->setIsDebugEnabled(true);
			rb->setUserData(gameobject.get());
			rb->setMass(settings.Mass);
			rb->setType((rp3d::BodyType)settings.BodyType);
			rb->enableGravity(settings.GravityEnabled);
			rb->setAngularDamping(settings.AngularDamping);
			rb->setLinearDamping(settings.LinearDamping);

			rb->setLinearLockAxisFactor(physics::utils::LockAxisToVextor3(settings.LinearLockAxis));
			rb->setAngularLockAxisFactor(physics::utils::LockAxisToVextor3(settings.AngularLockAxis));

			physics_component.SetRigidBody(rb);*/
		}
	}

	void World::Simulate(float dt)
	{
		float time_step = 1.f / 60.f;

		mAccumulatedTime += dt;

		while (mAccumulatedTime >= time_step)
		{
			mPhyxWorld->simulate(time_step);
			mPhyxWorld->fetchResults(true);
			mAccumulatedTime -= time_step;
		}
	}

	void World::SimulateEnd()
	{
		auto view = mRegistry.view<PhysicsComponent>();
		for (auto &e : view)
		{
			auto [physics_component] = view.get(e);
			if (!physics_component.Settings.PhysicsEnabled)
				continue;

			/*auto rb = (rp3d::RigidBody *)physics_component.GetRigidBody();
			mPhysicsWorld->destroyRigidBody(rb);*/
			// physx

			auto rb = (physx::PxRigidActor *)physics_component.GetRigidBody();
			rb->release();
			physics_component.SetRigidBody(nullptr);
		}

		mPhyxWorld->release();
		mPhyxWorld = nullptr;

		delete mSimulationEventCallback;
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
		if (mPhyxWorld)
		{
			const auto &rb = mPhyxWorld->getRenderBuffer();
			// for (physx::PxU32 i = 0; i < rb.getNbPoints(); i++)
			//{
			// }

			for (physx::PxU32 i = 0; i < rb.getNbLines(); i++)
			{
				const auto &line = rb.getLines()[i];

				uint32_t color = line.color1;
				glm::vec3 p0 = {line.pos0.x, line.pos0.y, line.pos0.z};
				glm::vec3 p1 = {line.pos1.x, line.pos1.y, line.pos1.z};
				LineRenderer::DrawLine(p0, p1, color, {});
			}

			for (physx::PxU32 i = 0; i < rb.getNbTriangles(); i++)
			{
				const auto &tri = rb.getTriangles()[i];
				glm::vec3 p0 = {tri.pos0.x, tri.pos0.y, tri.pos0.z};
				glm::vec3 p1 = {tri.pos1.x, tri.pos1.y, tri.pos1.z};
				glm::vec3 p2 = {tri.pos2.x, tri.pos2.y, tri.pos2.z};
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

	Ref<GameObject> World::CreateGameObject(const rttr::type &type)
	{
		auto obj = type.create({mRegistry.create(), this}).get_value<Ref<GameObject>>();
		AddGameObject(obj);
		return obj;
	}

	void World::AddGameObject(const Ref<GameObject> &object)
	{
		if (!object)
			return;

		const auto &id = object->GetID();
		mObjects.emplace(id, object);
		mEnttMap.emplace(*object, id);
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

	bool World::RayCast(
		const glm::vec3 &start, const glm::vec3 &dir, float maxDistance, FHitResult &result, uint16_t categoryMasks)
	{
		physx::PxRaycastBuffer hit;

		physx::PxVec3 origin = {start.x, start.y, start.z};
		physx::PxVec3 unitdir = {dir.x, dir.y, dir.z};
		bool status = mPhyxWorld->raycast(origin, unitdir, maxDistance, hit);
		if (status)
		{
			auto &block = hit.block;
			result.Normal = {block.normal.x, block.normal.y, block.normal.z};
			result.Position = {block.position.x, block.position.y, block.position.z};
			result.Object = (GameObject *)block.actor->userData;
			result.Component = (ColliderComponent *)block.shape->userData;
			result.Distance = block.distance;
			result.InitalOverlap = block.hadInitialOverlap();
		}

		return status;
	}

	std::pair<Camera *, FTransform> World::GetPrimaryCamera()
	{

		return {nullptr, FTransform()};
	}

	REFLECT(World)
	{
		BEGIN_REFLECT(World) REFLECT_CONSTRUCTOR();
	}
} // namespace BHive