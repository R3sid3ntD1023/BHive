#include "SphereComponent.h"
#include <reactphysics3d/reactphysics3d.h>
#include "physics/PhysicsContext.h"
#include "renderers/LineRenderer.h"
#include "scene/Entity.h"

namespace BHive
{
	void *SphereComponent::GetCollisionShape(const FTransform &world_transform)
	{
		return mCollisionShape = PhysicsContext::get_context().createSphereShape(
				   mRadius * glm::compMax(world_transform.get_scale()));
	}

	void SphereComponent::ReleaseCollisionShape()
	{
		PhysicsContext::get_context().destroySphereShape((rp3d::SphereShape *)mCollisionShape);
	}

	void SphereComponent::OnRender(SceneRenderer *renderer)
	{
		LineRenderer::DrawSphere(mRadius, 32, mOffset, mColor, GetWorldTransform());
	}

	void SphereComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ColliderComponent::Save(ar);
		ar(mRadius);
	}

	void SphereComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ColliderComponent::Load(ar);
		ar(mRadius);
	}

	REFLECT(SphereComponent)
	{
		BEGIN_REFLECT(SphereComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("Radius", mRadius);
	}
} // namespace BHive