#include "components/Components.h"
#include "GameObject.h"

namespace BHive
{
	GameObject::GameObject(const std::string &name, const entt::entity &handle, World *world)
		: mWorld(world),
		  mEntity(handle)
	{
		AddComponent<TagComponent>(name);
		AddComponent<TransformComponent>();
		AddComponent<IDComponent>();
		AddComponent<RelationshipComponent>();
	}

	const UUID &GameObject::GetID() const
	{
		return GetComponent<IDComponent>().ID;
	}

	const FTransform &GameObject::GetTransform() const
	{
		auto parentID = GetComponent<RelationshipComponent>().Parent;
		auto transform = GetComponent<TransformComponent>().Transform;

		if (parentID)
		{
			auto parent = mWorld->GetGameObject(parentID);

			return parent->GetTransform().to_mat4() * transform.to_mat4();
		}

		return transform;
	}
} // namespace BHive