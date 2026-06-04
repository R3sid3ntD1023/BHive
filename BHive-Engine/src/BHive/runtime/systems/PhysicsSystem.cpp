#include "PhysicsSystem.h"
#include "runtime/components/ColliderComponent.h"
#include "core/subsystem/SubSystem.h"
#include "physics/PhysicsContext.h"
#include <physx/PxPhysicsAPI.h>
#include "physics/PhysicsUtils.h"
#include "physics/EventListener.h"
#include "runtime/GameObject.h"
#include "runtime/World.h"
#include "gfx/renderers/LineRenderer.h"
#include "physics/HitResult.h"

namespace BHive
{
	void PhysicsSystem::Init(World *world)
	{
		// physx
		auto physics = (physx::PxPhysics *)GetSubSystem<PhysicsContext>().GetPhysics();

		physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

		// init callbacks
		mSimulationEventCallback = new SimulationCallback();

		mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = mCpuDispatcher;
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		mScene = physics->createScene(sceneDesc);
		mScene->setSimulationEventCallback(mSimulationEventCallback);

#ifdef _DEBUG
		mScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
		mScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 2.0f);

		mScene->setVisualizationParameter(physx::PxVisualizationParameter::eBODY_AXES, 2.0f);
		mScene->setVisualizationParameter(physx::PxVisualizationParameter::eBODY_MASS_AXES, 2.0f);
		mScene->setVisualizationParameter(physx::PxVisualizationParameter::eBODY_ANG_VELOCITY, 2.0f);
		mScene->setVisualizationParameter(physx::PxVisualizationParameter::eBODY_LIN_VELOCITY, 2.0f);

		mScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_DYNAMIC, 2.0f);
		mScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_AABBS, 2.0f);
		mScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_COMPOUNDS, 2.0f);
		mScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_EDGES, 2.0f);
		mScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_AXES, 2.0f);
#endif // _DEBUG

		physx::PxPvdSceneClient *pvdClient = mScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}

		auto view = world->GetRegistry().view<PhysicsComponent>();
		for (const auto &[e, component] : view.each())
		{
			auto &settings = component.Settings;

			if (!settings.PhysicsEnabled)
				continue;

			auto gameobject = world->GetGameObject((int32_t)e);
			auto t = gameobject->GetWorldTransform();

			physx::PxRigidActor *rigid_body = nullptr;
			switch (settings.BodyType)
			{
			case EBodyType::Static:
			{
				auto body = physics->createRigidStatic(PhysicsUtils::Convert(t));
				rigid_body = body;
			}
			break;
			case EBodyType::Dynamic:
			case EBodyType::Kinematic:
			{
				auto body = physics->createRigidDynamic(PhysicsUtils::Convert(t));
				body->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, settings.BodyType == EBodyType::Kinematic);
				body->setAngularDamping(settings.AngularDamping);
				body->setLinearDamping(settings.LinearDamping);
				body->setMass(settings.Mass);
				body->setRigidDynamicLockFlags(PhysicsUtils::GetLockFlags(settings.LinearLockAxis, settings.AngularLockAxis));
				body->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !settings.GravityEnabled);
				rigid_body = body;
			}
			break;
			default:
				break;
			}

			if (rigid_body && gameobject)
			{

#ifdef _DEBUG
				rigid_body->setActorFlag(PxActorFlag::eVISUALIZATION, true);
#endif // _DEBUG

				rigid_body->userData = gameobject.get();
				mScene->addActor(*rigid_body);
				component.SetRigidBody(rigid_body);
			}
		}
	}

	void PhysicsSystem::Update(float dt, World *world)
	{
		float time_step = 1.f / 60.f;

		mAccumulatedTime += dt;

		while (mAccumulatedTime >= time_step)
		{
			mScene->simulate(time_step);
			mScene->fetchResults(true);
			mAccumulatedTime -= time_step;
		}
	}

	void PhysicsSystem::Shutdown(World *world)
	{
		auto view = world->GetRegistry().view<PhysicsComponent>();
		for (const auto &[e, component] : view.each())
		{
			if (!component.Settings.PhysicsEnabled)
				continue;

			auto rb = (physx::PxRigidActor *)component.GetRigidBody();
			rb->release();
			component.SetRigidBody(nullptr);
		}

		mScene->release();
		mScene = nullptr;

		delete mSimulationEventCallback;
	}

	void PhysicsSystem::DebugDraw()
	{
		if (!mScene)
			return;

		const auto &rb = mScene->getRenderBuffer();

		for (physx::PxU32 i = 0; i < rb.getNbLines(); i++)
		{
			const auto &line = rb.getLines()[i];

			uint32_t color = line.color1;
			glm::vec3 p0 = {line.pos0.x, line.pos0.y, line.pos0.z};
			glm::vec3 p1 = {line.pos1.x, line.pos1.y, line.pos1.z};
			//LineRenderer::DrawLine(p0, p1, color, {});
		}

		for (physx::PxU32 i = 0; i < rb.getNbTriangles(); i++)
		{
			const auto &tri = rb.getTriangles()[i];
			glm::vec3 p0 = {tri.pos0.x, tri.pos0.y, tri.pos0.z};
			glm::vec3 p1 = {tri.pos1.x, tri.pos1.y, tri.pos1.z};
			glm::vec3 p2 = {tri.pos2.x, tri.pos2.y, tri.pos2.z};
			//LineRenderer::DrawTriangle(p0, p1, p2, tri.color0, {});
		}
	}

	void PhysicsSystem::InitCallbacks()
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

	bool PhysicsSystem::RayCast(const glm::vec3 &start, const glm::vec3 &dir, float maxDistance, FHitResult &result, uint16_t categoryMasks)
	{
		physx::PxRaycastBuffer hit;
		physx::PxQueryFilterData filter{};
		filter.data = {categoryMasks, 0, 0, 0};
		filter.flags = PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC;

		physx::PxVec3 origin = {start.x, start.y, start.z};
		physx::PxVec3 unitdir = {dir.x, dir.y, dir.z};
		bool status = mScene->raycast(origin, unitdir, maxDistance, hit, physx::PxHitFlag::eDEFAULT, filter);
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
} // namespace BHive