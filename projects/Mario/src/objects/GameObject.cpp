#include "components/IDComponent.h"
#include "components/RelationshipComponent.h"
#include "components/TagComponent.h"
#include "components/TransformComponent.h"
#include "GameObject.h"

namespace BHive
{
	GameObject::GameObject(World *world)
		: mWorld(world)
	{
		AddComponent<TagComponent>("New GameObject");
		AddComponent<TransformComponent>();
		AddComponent<IDComponent>();
		AddComponent<RelationshipComponent>();

		mPhysicsComponent.mOwningObject = this;
	}

	void GameObject::Begin()
	{
		mPhysicsComponent.Begin();

		for (auto &component : mComponents)
			component.second->Begin();
	}

	void GameObject::Update(float dt)
	{
		mPhysicsComponent.Update(dt);

		for (auto &component : mComponents)
			component.second->Update(dt);
	}

	void GameObject::Render()
	{
		for (auto &component : mComponents)
			component.second->Render();
	}

	void GameObject::End()
	{

		for (auto &component : mComponents)
			component.second->End();

		mPhysicsComponent.End();
	}

	PhysicsComponent &GameObject::GetPhysicsComponent()
	{
		return mPhysicsComponent;
	}

	void GameObject::SetName(const std::string &name)
	{
		GetComponent<TagComponent>().Name = name;
	}

	void GameObject::SetTransform(const FTransform &transform)
	{
		GetComponent<TransformComponent>().Transform = transform;
	}

	void GameObject::SetParent(GameObject *object)
	{
		if (!object)
			return;

		auto &this_rel = GetComponent<RelationshipComponent>();
		auto &other_rel = object->GetComponent<RelationshipComponent>();

		this_rel.Parent = object->GetID();
		other_rel.Children.insert(GetID());
	}

	void GameObject::AddChild(GameObject *object)
	{
		if (!object)
			return;

		auto &this_rel = GetComponent<RelationshipComponent>();
		auto &child_rel = object->GetComponent<RelationshipComponent>();

		this_rel.Children.insert(object->GetID());
		child_rel.Parent = GetID();
	}

	void GameObject::RemoveChild(GameObject *object)
	{
		if (!object)
			return;

		auto &this_rel = GetComponent<RelationshipComponent>();
		auto &child_rel = object->GetComponent<RelationshipComponent>();

		auto &child_id = object->GetID();
		if (this_rel.Children.contains(child_id))
		{
			this_rel.Children.erase(child_id);
			child_rel.Parent = 0;
		}
	}

	void GameObject::RemoveParent()
	{
		auto &this_rel = GetComponent<RelationshipComponent>();
		auto &id = GetID();

		if (this_rel.Parent)
		{
			auto parent = mWorld->GetGameObject(this_rel.Parent);
			auto &parent_rel = parent->GetComponent<RelationshipComponent>();

			if (parent_rel.Children.contains(id))
			{
				parent_rel.Children.erase(id);
			}
		}
	}

	void GameObject::Destroy()
	{
		mWorld->Destroy(GetID());
	}

	const UUID &GameObject::GetID() const
	{
		return GetComponent<IDComponent>().ID;
	}

	const std::string &GameObject::GetName() const
	{
		return GetComponent<TagComponent>().Name;
	}

	const FTransform &GameObject::GetTransform() const
	{
		auto &parentID = GetComponent<RelationshipComponent>().Parent;
		auto &transform = GetComponent<TransformComponent>().Transform;

		if (parentID)
		{
			auto parent = mWorld->GetGameObject(parentID);

			return parent->GetTransform().to_mat4() * transform.to_mat4();
		}

		return transform;
	}
	FTransform &GameObject::GetLocalTransform()
	{
		return GetComponent<TransformComponent>().Transform;
	}
	const FTransform &GameObject::GetLocalTransform() const
	{
		return GetComponent<TransformComponent>().Transform;
	}

	Ref<GameObject> GameObject::GetParent() const
	{
		auto &parent = GetComponent<RelationshipComponent>().Parent;
		if (!parent)
			return nullptr;

		return mWorld->GetGameObject(parent);
	}

	std::unordered_set<Ref<GameObject>> GameObject::GetChildren() const
	{
		std::unordered_set<Ref<GameObject>> children;
		auto &child_ids = GetComponent<RelationshipComponent>().Children;
		for (auto &id : child_ids)
			children.insert(mWorld->GetGameObject(id));

		return children;
	}

	REFLECT(GameObject)
	{
		BEGIN_REFLECT(GameObject) REFLECT_CONSTRUCTOR(World *);
	}
} // namespace BHive