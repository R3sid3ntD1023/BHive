#include "CapsuleComponent.h"
#include <reactphysics3d/reactphysics3d.h>
#include "scene/Entity.h"
#include "renderers/LineRenderer.h"
#include "physics/PhysicsContext.h"

namespace BHive
{
	void* CapsuleComponent::GetCollisionShape(const FTransform& world_transform)
	{
		auto radius = mRadius * glm::compMax(world_transform.get_scale());
		auto height = mHeight * glm::compMax(world_transform.get_scale());
		return mCollisionShape = PhysicsContext::get_context().createCapsuleShape(radius, height);
	}

	void CapsuleComponent::ReleaseCollisionShape()
	{
		PhysicsContext::get_context().destroyCapsuleShape((rp3d::CapsuleShape*)mCollisionShape);
	}

	void CapsuleComponent::OnRender(SceneRenderer* renderer)
	{
		LineRenderer::DrawCapsule(mRadius, mHeight, 16, mOffset, mColor, GetWorldTransform());
	}

	void CapsuleComponent::Serialize(StreamWriter& ar) const
	{
		ColliderComponent::Serialize(ar);
		ar(mHeight, mRadius);
	}

	void CapsuleComponent::Deserialize(StreamReader& ar)
	{
		ColliderComponent::Deserialize(ar);
		ar(mHeight, mRadius);
	}

	REFLECT(CapsuleComponent)
	{
		BEGIN_REFLECT(CapsuleComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("Radius", mRadius) REFLECT_PROPERTY("Height", mHeight);
	}
}  // namespace BHive