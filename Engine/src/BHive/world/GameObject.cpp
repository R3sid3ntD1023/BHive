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

	void GameObject::Begin()
	{
		for (auto &component : mComponents)
			component->Begin();
	}

	void GameObject::Update(float dt)
	{
		for (auto &component : mComponents)
		{
			if (component->IsTickEnabled())
				component->Update(dt);
		}
	}

	void GameObject::Render()
	{
		for (auto &component : mComponents)
			component->Render();
	}

	void GameObject::End()
	{
		for (auto it = mComponents.rbegin(); it != mComponents.rend(); it++)
		{
			auto component = (*it);
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

	void GameObject::SetTransform(const FTransform &transform)
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

	void GameObject::Save(cereal::JSONOutputArchive &ar) const
	{
		auto &components = GetComponents();

		ar.setNextName("Components");
		ar.startNode();
		ar(cereal::make_size_tag(components.size()));

		for (auto &component : components)
		{
			ar.startNode();
			ar(component->get_type());
			component->Save(ar);
			ar.finishNode();
		}

		ar.finishNode();
	}

	void GameObject::Load(cereal::JSONInputArchive &ar)
	{
		size_t num_components = 0;

		ar.setNextName("Components");
		ar.startNode();
		ar(cereal::make_size_tag(num_components));

		for (size_t i = 0; i < num_components; i++)
		{
			rttr::type component_type = BHive::InvalidType;

			ar.startNode();
			ar(component_type);

			Component *component = GetOrAddComponent(component_type);

			if (component)
				component->Load(ar);

			ar.finishNode();
		}

		ar.finishNode();
	}

	Component *GameObject::GetOrAddComponent(const rttr::type &type)
	{
		if (type.get_method(HAS_COMPONENT_FUNCTION_NAME).invoke({this}).to_bool())
		{
			return type.get_method(GET_COMPONENT_FUNCTION_NAME).invoke({this}).get_value<Component *>();
		}
		else
		{
			return type.get_method(ADD_COMPONENT_FUNCTION_NAME).invoke({this}).get_value<Component *>();
		}

		return nullptr;
	}

	void GameObject::AddComponent(Component *component)
	{
		component->SetOwner(this);
		mComponents.emplace_back(component);
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
			object->RemoveParent();
		}
		else
		{
			LOG_WARN("GameObject::RemoveChild: Object is not a child of this object");
		}
	}

	void GameObject::RemoveParent()
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
		OnDestroyedEvent.invoke(this);
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

	FTransform GameObject::GetTransform() const
	{
		auto &transform = GetComponent<TransformComponent>()->Transform;

		if (auto parent = GetParent())
		{
			return parent->GetTransform().to_mat4() * transform.to_mat4();
		}

		return transform;
	}
	FTransform &GameObject::GetLocalTransform()
	{
		return GetComponent<TransformComponent>()->Transform;
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

	REFLECT(GameObject)
	{
		BEGIN_REFLECT(GameObject)
		REFLECT_CONSTRUCTOR(const entt::entity &, World *);
	}
} // namespace BHive