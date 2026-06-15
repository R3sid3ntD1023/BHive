#include "components/RelationshipComponent.h"
#include "components/TagComponent.h"
#include "components/TransformComponent.h"
#include "GameObject.h"

namespace BHive
{
	GameObject::GameObject(const entt::entity &handle, World *world)
		: mEntity(handle),
		  mWorld(world)
	{
		AddComponent<TagComponent>();
		AddComponent<TransformComponent>();
		AddComponent<RelationshipComponent>();
	}

	GameObject::GameObject(const std::string &name, const entt::entity &handle, World *world)
		: GameObject(handle, world)
	{
		SetName(name);
	}

	void GameObject::Begin()
	{
		for (auto &component : mComponents)
		{
			if (auto tickable = Cast<ITickable>(component))
				mTickableComponents.push_back(tickable);

			component->Begin();
		}
	}

	void GameObject::Update(float dt)
	{
		if (!IsTickEnabled())
			return;

		for (auto &tickable : mTickableComponents)
		{
			if (tickable->IsTickEnabled())
				tickable->Update(dt);
		}
	}

	void GameObject::End()
	{
		for (auto component : mComponents)
		{
			component->End();
		}
	}

	PhysicsComponent *GameObject::GetPhysicsComponent()
	{
		if (HasComponent<PhysicsComponent>())
			return GetComponent<PhysicsComponent>();

		return nullptr;
	}

	void GameObject::SetName(const std::string &name)
	{
		GetComponent<TagComponent>()->Name = name;
	}

	void GameObject::SetWorldTransform(const FTransform &transform)
	{
		if (auto parent = GetParent())
		{
			auto parent_transform = parent->GetWorldTransform();
			SetLocalTransform(parent_transform.Inverse().ToMat4() * transform.ToMat4());
			return;
		}

		SetLocalTransform(transform);
	}

	void GameObject::SetLocalTransform(const FTransform &transform)
	{
		GetComponent<TransformComponent>()->Transform = transform;
	}

	void GameObject::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mComponents.size());

		for (auto component : mComponents)
		{
			ar(component->get_type());
			component->Save(ar);
		}
	}

	void GameObject::Load(cereal::BinaryInputArchive &ar)
	{
		size_t num_components = 0;
		ar(num_components);

		for (size_t i = 0; i < num_components; i++)
		{
			rttr::type type = InvalidType;

			ar(type);

			Component *component = GetOrAddComponent(type);

			if (component)
			{
				component->Load(ar);
			}
		}
	}

	Component *GameObject::GetOrAddComponent(const rttr::type &type)
	{
		if (!type)
			return nullptr;

		if (type.get_method(HAS_COMPONENT_FUNCTION_NAME).invoke({this}).to_bool())
		{
			return type.get_method(GET_COMPONENT_FUNCTION_NAME).invoke({this}).get_value<Component *>();
		}

		return type.get_method(ADD_COMPONENT_FUNCTION_NAME).invoke({this}).get_value<Component *>();
	}

	void GameObject::AddComponent(Component *component)
	{
		component->SetOwner(this);
		mComponents.insert(component);
	}

	void GameObject::SetParent(GameObject *object)
	{
		if (!object)
			return;

		auto rel = GetComponent<RelationshipComponent>();
		auto other = object->GetComponent<RelationshipComponent>();

		rel->Parent = object->GetID();
		other->Children.insert(GetID());
	}

	void GameObject::AddChild(GameObject *object)
	{
		if (!object)
			return;

		object->SetParent(this);
	}

	void GameObject::RemoveChild(GameObject *object)
	{
		if (!object)
			return;

		auto rel = GetComponent<RelationshipComponent>();
		if (rel->Children.contains(object->GetID()))
		{
			object->RemoveFromParent();
		}
		else
		{
			LOG_WARN("GameObject::RemoveChild: Object is not a child of this object");
		}
	}

	void GameObject::RemoveFromParent()
	{
		auto rel = GetComponent<RelationshipComponent>();

		if (!rel->Parent)
			return;

		auto parent = mWorld->GetGameObject(rel->Parent);
		auto parent_rel = parent->GetComponent<RelationshipComponent>();

		rel->Parent = NullID;
		parent_rel->Children.erase(GetID());
	}

	void GameObject::Destroy()
	{
		RemoveFromParent();
		OnDestroyedEvent.invoke(this);
		mIsPendingDestroy = true;
		mWorld->Destroy(GetID());
	}

	const UUID &GameObject::GetID() const
	{
		return GetComponent<TagComponent>()->GetID();
	}

	const std::string &GameObject::GetName() const
	{
		return GetComponent<TagComponent>()->Name;
	}

	FTransform GameObject::GetWorldTransform() const
	{
		auto &transform = GetComponent<TransformComponent>()->Transform;

		if (auto parent = GetParent())
		{
			return parent->GetWorldTransform().ToMat4() * transform.ToMat4();
		}

		return transform;
	}
	FTransform &GameObject::GetLocalTransform()
	{
		return GetComponent<TransformComponent>()->Transform;
	}

	AABB GameObject::GetBounds() const
	{
		FSphere sphere{};
		sphere.Radius = 10.f;
		sphere.Origin = {};
		return sphere;
	}

	const FTransform &GameObject::GetLocalTransform() const
	{
		return GetComponent<TransformComponent>()->Transform;
	}

	Ref<GameObject> GameObject::GetParent() const
	{
		auto parent = GetComponent<RelationshipComponent>()->Parent;

		if (!parent)
			return nullptr;

		return mWorld->GetGameObject(parent);
	}

	const uint64_t GameObject::GetGroup() const
	{
		return GetComponent<TagComponent>()->Group;
	}

	bool GameObject::IsInGroup(uint16_t group) const
	{
		auto groups = GetComponent<TagComponent>()->Group;
		return (groups & group) != 0;
	}

	std::unordered_set<Ref<GameObject>> GameObject::GetChildren() const
	{
		auto rel = GetComponent<RelationshipComponent>();
		std::unordered_set<Ref<GameObject>> children;
		auto &child_ids = rel->Children;
		for (auto &id : child_ids)
			children.insert(mWorld->GetGameObject(id));

		return children;
	}

	bool GameObject::HasChildren() const
	{
		return GetComponent<RelationshipComponent>()->Children.size() > 0;
	}

	REFLECT(GameObject)
	{
		BEGIN_REFLECT(GameObject)
		REFLECT_CONSTRUCTOR(const entt::entity &, World *);
	}
} // namespace BHive