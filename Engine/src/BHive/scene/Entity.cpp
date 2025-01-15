#include "Component.h"
#include "Entity.h"
#include "physics/PhysicsUtils.h"
#include "World.h"

namespace BHive
{

	Entity::Entity()
	{
	}

	void Entity::OnBegin()
	{
		if (mPhysicsComponent.mPhysicsEnabled)
		{
			auto rb = GetWorld()->GetPhysicsWorld()->createRigidBody(
				physics::utils::GetPhysicsTransform(GetWorldTransform()));
			rb->setIsDebugEnabled(true);
			rb->setUserData(this);
			rb->setMass(mPhysicsComponent.mMass);
			rb->setType((rp3d::BodyType)mPhysicsComponent.mBodyType);
			rb->enableGravity(mPhysicsComponent.mGravityEnabled);
			rb->setAngularDamping(mPhysicsComponent.mAngularDamping);
			rb->setLinearDamping(mPhysicsComponent.mLinearDamping);

			rb->setLinearLockAxisFactor(
				physics::utils::LockAxisToVextor3(mPhysicsComponent.mLinearLockAxis));
			rb->setAngularLockAxisFactor(
				physics::utils::LockAxisToVextor3(mPhysicsComponent.mAngularLockAxis));

			mPhysicsComponent.SetRigidBody(rb);
		}

		for (auto &component : mComponents)
			component->OnBegin();
	}

	void Entity::OnUpdate(float dt)
	{
		if (IsTickEnabled())
		{
			for (auto &component : mComponents)
			{
				if (!component->IsTickEnabled())
					continue;

				component->OnUpdate(dt);
			}
		}

		if (mPhysicsComponent.mPhysicsEnabled)
		{
			auto rb = (rp3d::RigidBody *)mPhysicsComponent.GetRigidBody();

			if (mPhysicsComponent.mBodyType == EBodyType::Dynamic)
			{

				auto &transform = rb->getTransform();
				auto scale = mTransform.get_scale();
				mTransform = physics::utils::GetTransform(transform, scale);
			}
			else
			{
				rb->setTransform(physics::utils::GetPhysicsTransform(mTransform));
			}
		}
	}

	void Entity::OnEnd()
	{
		for (auto &component : mComponents)
			component->OnEnd();

		if (mPhysicsComponent.mPhysicsEnabled)
		{
			GetWorld()->GetPhysicsWorld()->destroyRigidBody(
				(rp3d::RigidBody *)mPhysicsComponent.GetRigidBody());
		}
	}

	void Entity::Destroy(bool destroy_decendents)
	{
		auto children = GetChildren();
		for (auto child : children)
		{
			child->DetachFromParent();
			if (destroy_decendents)
			{
				child->Destroy(destroy_decendents);
			}
		}

		DetachFromParent();

		OnEntityDestroyed.invoke(this);
	}

	void Entity::AddComponent(const ComponentPtr &component)
	{
		mComponents.push_back(component);

		RegisterComponent(component.get());
	}

	void Entity::RemoveComponent(Component *component_ptr)
	{
		auto it = std::find_if(
			mComponents.begin(), mComponents.end(),
			[component_ptr](const auto &component) { return component.get() == component_ptr; });

		if (it == mComponents.end())
			return;

		mComponents.erase(it);
	}

	Ref<Entity> Entity::Copy() const
	{
		auto new_entity = new Entity(*this);
		new_entity->mComponents.clear();

		for (auto &component : mComponents)
		{
			auto type = component->get_type();
			rttr::variant copied =
				type.get_method(COPY_COMPONENT_FUNC_NAME).invoke({}, component.get());
			if (copied)
			{
				auto copied_component = copied.get_value<Ref<Component>>();
				new_entity->AddComponent(copied_component);
			}
		}
		return Ref<Entity>(new_entity);
	}

	Ref<Entity> Entity::Duplicate(bool duplicate_children)
	{
		auto new_entity = new Entity();
		new_entity->SetLocalTransform(GetLocalTransform());
		new_entity->SetName(GetName());

		for (auto &component : mComponents)
		{
			auto type = component->get_type();
			rttr::variant duplicated =
				type.get_method(DUPLICATE_COMPONENT_FUNC_NAME).invoke({}, component.get());
			if (duplicated)
			{
				auto duplicated_component = duplicated.get_value<Ref<Component>>();
				new_entity->AddComponent(duplicated_component);
			}
		}

		if (duplicate_children)
		{
			auto children = GetChildren();
			for (auto &child : children)
			{
				auto duplicated_child = mWorld->DuplicateEntity(child);
				duplicated_child->AttachTo(new_entity);
			}
		}

		return Ref<Entity>(new_entity);
	}

	void Entity::RegisterComponents()
	{
		for (auto &component : mComponents)
		{
			RegisterComponent(component.get());
		}
	}

	void Entity::RegisterComponent(Component *component)
	{
		if (!component)
			return;

		component->mOwningentity = this;
	}

	void Entity::SetLocalTransform(const FTransform &transform)
	{
		mTransform = transform;
	}

	const FTransform &Entity::GetLocalTransform() const
	{
		return mTransform;
	}

	void Entity::SetWorldTransform(const FTransform &transform)
	{
		if (GetParent())
		{
			mTransform = GetParent()->GetWorldTransform().inverse() * transform;
			return;
		}

		mTransform = transform;
	}

	FTransform Entity::GetWorldTransform() const
	{
		if (GetParent())
			return GetParent()->GetWorldTransform() * mTransform;

		return mTransform;
	}

	Entity *Entity::GetParent() const
	{
		auto parent_id = mRelationshipComponent.GetParentID();

		if (parent_id)
			return mWorld->GetEntities().at(parent_id).get();

		return nullptr;
	}

	void Entity::AttachTo(Entity *entity)
	{
		if (!entity)
			return;

		if (GetParent())
			DetachFromParent();

		mRelationshipComponent.SetParentID(entity->GetUUID());
		entity->mRelationshipComponent.AddChild(GetUUID());
	}

	void Entity::DetachFromParent()
	{
		auto parent = GetParent();

		if (!parent)
			return;

		parent->mRelationshipComponent.RemoveChild(GetUUID());
		mRelationshipComponent.SetParentID(0);
	}

	EntityChildren Entity::GetChildren() const
	{
		EntityChildren children;

		const auto &entities = mWorld->GetEntities();

		for (auto &child_id : mRelationshipComponent.GetChildren())
		{
			auto entity = entities.at(child_id);
			if (entity)
				children.push_back(entity.get());
		}

		return children;
	}

	void Entity::SetTickEnabled(bool enabled)
	{
		mTickEnabled = enabled;
	}

	void Entity::Save(cereal::BinaryOutputArchive &ar) const
	{
		ObjectBase::Save(ar);

		ar(mTickEnabled, mTransform, mRelationshipComponent, mPhysicsComponent, mComponents.size());

		for (auto &component : mComponents)
		{
			ar(component->get_type());
			component->Save(ar);
		}
	}

	void Entity::Load(cereal::BinaryInputArchive &ar)
	{
		ObjectBase::Load(ar);

		size_t num_components = 0;

		ar(mTickEnabled, mTransform, mRelationshipComponent, mPhysicsComponent, num_components);

		if (mComponents.size() < num_components)
			mComponents.resize(num_components);

		for (size_t i = 0; i < num_components; i++)
		{
			AssetType component_type = InvalidType;
			ar(component_type);

			auto &component = mComponents[i];
			if (!component)
			{
				component = component_type.create().get_value<Ref<Component>>();
			}

			component->Load(ar);
		}

		RegisterComponents();
	}

	bool Entity::operator==(const Entity &rhs) const
	{
		return GetUUID() == rhs.GetUUID();
	}

	bool Entity::operator!=(const Entity &rhs) const
	{
		return !(*this == rhs);
	}

	REFLECT(Entity)
	{
		BEGIN_REFLECT(Entity)
		REFLECT_CONSTRUCTOR()
		REFLECT_CONSTRUCTOR(const Entity &)
		REFLECT_PROPERTY("Physics", mPhysicsComponent)
		REFLECT_METHOD("AddComponent", &Entity::AddComponent);
	}
} // namespace BHive
