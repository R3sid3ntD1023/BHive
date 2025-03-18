#include "BoxComponent.h"
#include "physics/PhysicsContext.h"
#include "renderers/Renderer.h"
#include "objects/GameObject.h"

namespace BHive
{
	void BoxComponent::Render()
	{
		auto object = GetOwner();
		auto t = object->GetTransform();
		LineRenderer::DrawBox(Extents, Offset, Color, t);
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
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Extents)
			REFLECT_METHOD(ADD_COMPONENT_FUNCTION_NAME, &GameObject::AddComponent<BoxComponent>);
	}

} // namespace BHive