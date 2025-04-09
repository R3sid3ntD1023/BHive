#include "ColliderComponent.h"
#include "GameObject.h"
#include "physics/PhysicsContext.h"
#include "Physics/PhysicsCore.h"

namespace BHive
{
	void ColliderComponent::Begin()
	{
		auto object = GetOwner();
		auto physc = object->GetPhysicsComponent();

		if (!physc)
			return;

		auto rb = physc->GetRigidBody();

		CreateCollsionShape(rb, object->GetLocalTransform());
	}

	void ColliderComponent::Update(float)
	{
	}

	void ColliderComponent::End()
	{
		auto object = GetOwner();
		auto physc = object->GetPhysicsComponent();

		if (!physc)
			return;

		auto rb = physc->GetRigidBody();
		ReleaseCollisionShape(rb);
	}
	void ColliderComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(CollisionEnabled, Offset, Color, IsTrigger, CollisionChannel, CollisionChannelMasks,
		   TAssetHandle(PhysicsMaterial));
	}

	void ColliderComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(CollisionEnabled, Offset, Color, IsTrigger, CollisionChannel, CollisionChannelMasks,
		   TAssetHandle(PhysicsMaterial));
	}

	void BHive::ColliderComponent::CreateCollsionShape(void *rb, const FTransform &transform)
	{
		if (!CollisionEnabled)
			return;

		auto geo = (physx::PxGeometry *)GetGeometry();
		if (!geo)
			return;

		auto physcs = PhysicsContext::GetPhysics();
		physx::PxMaterial *material = physcs->createMaterial(1.f, 1.0f, 0.f);

		if (PhysicsMaterial)
		{
			auto friction = PhysicsMaterial->mFrictionCoefficient;
			auto resitution = PhysicsMaterial->mBounciness;
			material->setStaticFriction(friction);
			material->setDynamicFriction(friction);
			material->setRestitution(resitution);
		}

		physx::PxFilterData filter_data(CollisionChannel, CollisionChannelMasks, 0, 0);

		auto shape = physcs->createShape(*geo, *material, true);

#ifdef _DEBUG
		shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);
#endif // _DEBUG
		((physx::PxRigidActor *)rb)->attachShape(*shape);
		shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, IsTrigger);
		shape->userData = this;
		shape->setLocalPose(physics::utils::Convert(Offset));
		shape->setQueryFilterData(filter_data);
		shape->setSimulationFilterData(filter_data);

		mCollisionShape = shape;
		material->release();
		delete geo;
	}

	void ColliderComponent::ReleaseCollisionShape(void *rb)
	{
		if (!CollisionEnabled)
			return;

		auto shape = (physx::PxShape *)mCollisionShape;
		((physx::PxRigidActor *)rb)->detachShape(*shape);
	}

	REFLECT(ColliderComponent)
	{
		BEGIN_REFLECT(ColliderComponent)
		REFLECT_PROPERTY(CollisionEnabled)
		REFLECT_PROPERTY(Offset)
		REFLECT_PROPERTY(Color)
		REFLECT_PROPERTY(IsTrigger)
		REFLECT_PROPERTY(CollisionChannel)
		REFLECT_PROPERTY(CollisionChannelMasks) REFLECT_PROPERTY(PhysicsMaterial);
	}

	REFLECT(ECollisionChannel)
	{
		BEGIN_REFLECT_ENUM(ECollisionChannel)
		(ENUM_VALUE(CollisionChannel_None), ENUM_VALUE(CollisionChannel_0), ENUM_VALUE(CollisionChannel_1),
		 ENUM_VALUE(CollisionChannel_2), ENUM_VALUE(CollisionChannel_3), ENUM_VALUE(CollisionChannel_4),
		 ENUM_VALUE(CollisionChannel_5), ENUM_VALUE(CollisionChannel_6), ENUM_VALUE(CollisionChannel_7),
		 ENUM_VALUE(CollisionChannel_8), ENUM_VALUE(CollisionChannel_9), ENUM_VALUE(CollisionChannel_10),
		 ENUM_VALUE(CollisionChannel_11), ENUM_VALUE(CollisionChannel_12), ENUM_VALUE(CollisionChannel_13),
		 ENUM_VALUE(CollisionChannel_14), ENUM_VALUE(CollisionChannel_All));
	}
} // namespace BHive