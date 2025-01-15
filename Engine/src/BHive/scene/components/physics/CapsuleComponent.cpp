#include "CapsuleComponent.h"
#include "physics/PhysicsContext.h"
#include "renderers/LineRenderer.h"
#include <reactphysics3d/reactphysics3d.h>

namespace BHive
{
	void *CapsuleComponent::GetCollisionShape(const FTransform &world_transform)
	{
		auto radius = mRadius * glm::compMax(world_transform.get_scale());
		auto height = mHeight * glm::compMax(world_transform.get_scale());
		return mCollisionShape = PhysicsContext::get_context().createCapsuleShape(radius, height);
	}

	void CapsuleComponent::ReleaseCollisionShape()
	{
		PhysicsContext::get_context().destroyCapsuleShape((rp3d::CapsuleShape *)mCollisionShape);
	}

	void CapsuleComponent::OnRender(SceneRenderer *renderer)
	{
		LineRenderer::DrawCapsule(mRadius, mHeight, 16, mOffset, mColor, GetWorldTransform());
	}

	void CapsuleComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ColliderComponent::Save(ar);
		ar(mHeight, mRadius);
	}

	void CapsuleComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ColliderComponent::Load(ar);
		ar(mHeight, mRadius);
	}

	REFLECT(CapsuleComponent)
	{
		BEGIN_REFLECT(CapsuleComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("Radius", mRadius) REFLECT_PROPERTY("Height", mHeight);
	}
} // namespace BHive