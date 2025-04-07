#include "GameObject.h"

namespace BHive
{
	GameObject::GameObject(const entt::entity &handle, World *world)
		: mEntity(handle),
		  mWorld(world)
	{
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

	PhysicsComponent &GameObject::GetPhysicsComponent()
	{
		return *GetComponent<PhysicsComponent>();
	}

	void GameObject::SetName(const std::string &name)
	{
		mName = name;
	}

	void GameObject::SetTransform(const FTransform &transform)
	{
		mTransform = transform;
	}

	void GameObject::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mID, mName, mTransform, mParent, mChildren);
		ar(mComponents.size());

		for (auto component : mComponents)
		{
			ar(component->get_type());
			component->Save(ar);
		}
	}

	void GameObject::Load(cereal::BinaryInputArchive &ar)
	{
		ar(mID, mName, mTransform, mParent, mChildren);
		size_t num_components = 0;
		ar(num_components);

		for (size_t i = 0; i < num_components; i++)
		{
			rttr::type type = InvalidType;

			ar(type);

			Component *component = nullptr;
			if (type.get_method(HAS_COMPONENT_FUNCTION_NAME).invoke({this}).to_bool())
			{
				component = type.get_method(GET_COMPONENT_FUNCTION_NAME).invoke({this}).get_value<Component *>();
			}
			else
			{
				component = type.get_method(ADD_COMPONENT_FUNCTION_NAME).invoke({this}).get_value<Component *>();
			}

			if (component)
			{
				component->Load(ar);
			}
		}
	}

	void GameObject::Save(cereal::JSONOutputArchive &ar) const
	{
		ar(MAKE_NVP("ID", mID), MAKE_NVP("Name", mName), MAKE_NVP("Transform", mTransform), MAKE_NVP("Parent", mParent));
	}

	void GameObject::Load(cereal::JSONInputArchive &ar)
	{
		ar(MAKE_NVP("ID", mID), MAKE_NVP("Name", mName), MAKE_NVP("Transform", mTransform), MAKE_NVP("Parent", mParent));
	}

	void GameObject::AddComponent(Component *component)
	{
		component->SetOwner(this);
		mComponents.emplace_back(component);
	}

	void GameObject::SetParent(GameObject *object)
	{
		mParent = object->GetID();
		object->mChildren.insert(GetID());
	}

	void GameObject::AddChild(GameObject *object)
	{
		if (!object)
			return;

		object->SetParent(this);
		mChildren.insert(object->GetID());
	}

	void GameObject::RemoveChild(GameObject *object)
	{
		if (!object)
			return;

		if (mChildren.contains(object->GetID()))
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
		auto parent = mWorld->GetGameObject(mParent);
		mParent = NullID;
		parent->mChildren.erase(GetID());
	}

	void GameObject::Destroy()
	{
		OnDestroyedEvent.invoke(this);
		mWorld->Destroy(GetID());
	}

	const UUID &GameObject::GetID() const
	{
		return mID;
	}

	const std::string &GameObject::GetName() const
	{
		return mName;
	}

	const FTransform &GameObject::GetTransform() const
	{

		if (mParent)
		{
			auto parent = mWorld->GetGameObject(mParent);

			return parent->GetTransform().to_mat4() * mTransform.to_mat4();
		}

		return mTransform;
	}
	FTransform &GameObject::GetLocalTransform()
	{
		return mTransform;
	}
	const FTransform &GameObject::GetLocalTransform() const
	{
		return mTransform;
	}

	Ref<GameObject> GameObject::GetParent() const
	{
		if (!mParent)
			return nullptr;

		return mWorld->GetGameObject(mParent);
	}

	std::unordered_set<Ref<GameObject>> GameObject::GetChildren() const
	{
		std::unordered_set<Ref<GameObject>> children;
		auto &child_ids = mChildren;
		for (auto &id : child_ids)
			children.insert(mWorld->GetGameObject(id));

		return children;
	}

	RTTR_REGISTRATION
	{
		BEGIN_REFLECT(FTransform)
			.property(
				"Translation", &FTransform::get_translation,
				rttr::select_overload<void(const glm::vec3 &)>(&FTransform::set_translation))
				REFLECT_PROPERTY("Rotation", get_rotation, set_rotation) REFLECT_PROPERTY("Scale", get_scale, set_scale);
	}

	REFLECT(GameObject)
	{
		BEGIN_REFLECT(GameObject)
		REFLECT_CONSTRUCTOR(const entt::entity &, World *)
		REFLECT_PROPERTY("ID", mID)
		REFLECT_PROPERTY("Name", mName)
		REFLECT_PROPERTY("Transform", mTransform)
		REFLECT_PROPERTY_READ_ONLY("Parent", mParent) REFLECT_PROPERTY_READ_ONLY("Children", mChildren);
	}
} // namespace BHive