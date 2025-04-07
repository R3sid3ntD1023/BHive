#include "BoxComponent.h"
#include "GameObject.h"
#include "physics/PhysicsContext.h"
#include "renderers/Renderer.h"

namespace BHive
{
	void BoxComponent::Render()
	{
		auto object = GetOwner();
		auto t = object->GetTransform();
		LineRenderer::DrawBox(Extents, Offset, Color, t);
	}

	void BoxComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ColliderComponent::Save(ar);
		ar(Extents);
	}

	void BoxComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ColliderComponent::Load(ar);
		ar(Extents);
	}

	void *BoxComponent::GetCollisionShape(const FTransform &world_transform)
	{
		auto extents = Extents * world_transform.get_scale();
		return PhysicsContext::get_context().createBoxShape(rp3d::Vector3{extents.x, extents.y, extents.z});
	}

	void BoxComponent::OnReleaseCollisionShape()
	{
		PhysicsContext::get_context().destroyBoxShape((rp3d::BoxShape *)mCollisionShape);
		mCollisionShape = nullptr;
	}

	REFLECT(BoxComponent)
	{
		BEGIN_REFLECT(BoxComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Extents) COMPONENT_IMPL();
	}

} // namespace BHive