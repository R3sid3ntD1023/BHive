#include "BoxComponent.h"
#include <reactphysics3d/reactphysics3d.h>
#include "scene/Entity.h"
#include "renderers/LineRenderer.h"
#include "physics/PhysicsContext.h"

namespace BHive
{
	void *BoxComponent::GetCollisionShape(const FTransform &world_transform)
	{
		auto extents = mExtents * world_transform.get_scale();
		return mCollisionShape = PhysicsContext::get_context().createBoxShape(
				   rp3d::Vector3{extents.x, extents.y, extents.z});
	}

	void BoxComponent::ReleaseCollisionShape()
	{
		PhysicsContext::get_context().destroyBoxShape((rp3d::BoxShape *)mCollisionShape);
	}

	void BoxComponent::OnRender(SceneRenderer *renderer)
	{
		LineRenderer::DrawBox(mExtents, mOffset, mColor, GetWorldTransform());
	}

	void BoxComponent::Save(cereal::JSONOutputArchive &ar) const
	{
		ColliderComponent::Save(ar);
		ar(MAKE_NVP("Extents", mExtents));
	}

	void BoxComponent::Load(cereal::JSONInputArchive &ar)
	{
		ColliderComponent::Load(ar);
		ar(MAKE_NVP("Extents", mExtents));
	}

	REFLECT(BoxComponent)
	{
		BEGIN_REFLECT(BoxComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("Extents", mExtents);
	}

} // namespace BHive