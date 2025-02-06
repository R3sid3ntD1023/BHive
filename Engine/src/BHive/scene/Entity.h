#pragma once

#include "asset/AssetType.h"
#include "Component.h"
#include "interfaces/ITickable.h"
#include "interfaces/ITransform.h"
#include "ObjectBase.h"
#include "scene/components/IPhysicsComponent.h"
#include "scene/components/RelationshipComponent.h"
#include "core/serialization/Serialization.h"

namespace BHive
{
	class Component;
	class SceneComponent;
	struct ColliderComponent;
	class World;
	class Entity;

	using ComponentPtr = Ref<Component>;
	using ComponentList = std::vector<Ref<Component>>;
	using EntityChildren = std::vector<Entity *>;

	class Entity : public ObjectBase, public ITickable, public ITransform
	{
	public:
		Entity();

		virtual ~Entity() = default;

		virtual void OnBegin();

		virtual void OnUpdate(float /*deltatime*/);

		virtual void OnEnd();

		void Destroy(bool decendents = true);

		template <typename T>
		Ref<T> AddNewComponent(const std::string &name)
		{
			auto component = CreateRef<T>();
			component->SetName(name);

			AddComponent(component);

			return component;
		}

		Ref<Entity> Copy() const;

		Ref<Entity> Duplicate(bool duplicate_children = false);

		IPhysicsComponent &GetPhysicsComponent() { return mPhysicsComponent; }

		void AddComponent(const ComponentPtr &component);

		void RemoveComponent(Component *component);

		void SetLocalTransform(const FTransform &transform);

		void SetWorldTransform(const FTransform &transform);

		const FTransform &GetLocalTransform() const;

		FTransform GetWorldTransform() const;

		Entity *GetParent() const;

		void AttachTo(Entity *entity);

		void DetachFromParent();

		ComponentList &GetComponents() { return mComponents; }

		const ComponentList &GetComponents() const { return mComponents; }

		EntityChildren GetChildren() const;

		World *GetWorld() { return mWorld; }

		/*ITickable*/

		virtual bool IsTickEnabled() const override { return mTickEnabled; };

		virtual void SetTickEnabled(bool) override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		bool operator==(const Entity &rhs) const;

		bool operator!=(const Entity &rhs) const;

	private:
		void RegisterComponents();
		void RegisterComponent(Component *component);

	private:
		FTransform mTransform;

		RelationshipComponent mRelationshipComponent;
		IPhysicsComponent mPhysicsComponent;

		ComponentList mComponents;

		bool mTickEnabled{true};

		World *mWorld = nullptr;

		REFLECTABLEV(ObjectBase, ITickable, ITransform)

		friend class World;
	};

	REFLECT_EXTERN(Entity)

} // namespace BHive
