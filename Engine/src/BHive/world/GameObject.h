#pragma once

#include "components/PhysicsComponent.h"
#include "core/Core.h"
#include "core/EventDelegate.h"
#include "core/serialization/Serialization.h"
#include "core/UUID.h"
#include "math/Transform.h"
#include "World.h"
#include "world/Component.h"

namespace BHive
{
	DECLARE_EVENT(FOnDestroyed, GameObject *);

	using ComponentList = std::vector<Ref<Component>>;

	struct GameObject
	{
		GameObject(World *world);
		GameObject(const GameObject &) = default;

		virtual void Begin();
		virtual void Update(float dt);
		virtual void Render();
		virtual void End();

		template <typename T, typename... TArgs>
		Ref<T> AddComponent(TArgs &&...args)
		{
			auto component = CreateRef<T>(std::forward<TArgs>(args)...);
			AddComponent(component);
			return component;
		}

		template <typename T>
		Ref<T> EmplaceOrReplaceComponent(const T &component)
		{
			return nullptr;
		}

		void AddComponent(const Ref<Component> &component);
		void RemoveComponent(const Ref<Component> &component);
		void RemoveComponent(Component *component);

		template <typename T>
		bool HasComponent()
		{
			auto it = std::find_if(
				mComponents.begin(), mComponents.end(),
				[](const Ref<Component> &comp) { return comp && comp->get_type() == rttr::type::get<T>(); });

			return it != mComponents.end();
		}

		template <typename T>
		Ref<T> GetComponent()
		{
			auto it = std::find_if(
				mComponents.begin(), mComponents.end(),
				[](const Ref<Component> &comp) { return comp && comp->get_type() == rttr::type::get<T>(); });

			return Cast<T>(*it);
		}

		template <typename T>
		void RemoveComponent()
		{
			auto it = std::find_if(
				mComponents.begin(), mComponents.end(),
				[](const Ref<Component> &comp) { return comp && comp->get_type() == rttr::type::get<T>(); });

			if (it != mComponents.end())
			{
				(*it)->End();
				mComponents.erase(it);
			}
		}

		void SetName(const std::string &name);
		void SetParent(const BHive::UUID &parent);

		PhysicsComponent &GetPhysicsComponent();
		void SetTransform(const FTransform &transform);

		virtual void Save(cereal::BinaryOutputArchive &ar) const;
		virtual void Load(cereal::BinaryInputArchive &ar);

		virtual void Save(cereal::JSONOutputArchive &ar) const;
		virtual void Load(cereal::JSONInputArchive &ar);

		void SetParent(GameObject *object);
		void AddChild(GameObject *object);
		void RemoveChild(GameObject *object);
		void RemoveParent();
		void Destroy();

		const UUID &GetID() const;
		const std::string &GetName() const;
		const FTransform &GetTransform() const;
		FTransform &GetLocalTransform();
		const FTransform &GetLocalTransform() const;

		World *GetWorld() const { return mWorld; }
		Ref<GameObject> GetParent() const;
		std::unordered_set<Ref<GameObject>> GetChildren() const;
		const ComponentList &GetComponents() const { return mComponents; }
		ComponentList &GetComponents() { return mComponents; }

		FOnDestroyedEvent OnDestroyedEvent;

		REFLECTABLEV()

	protected:
		World *mWorld = nullptr;
		UUID mID;
		std::string mName = "New GameObject";
		FTransform mTransform;

		ComponentList mComponents;
		Ref<PhysicsComponent> mPhysicsComponent;

		UUID mParent = NullID;
		std::unordered_set<UUID> mChildren;
	};
} // namespace BHive

#define ADD_COMPONENT_FUNCTION_NAME "AddComponent"
#define REMOVE_COMPONENT_FUNCTION_NAME "RemoveComponent"
#define HAS_COMPONENT_FUNCTION_NAME "HasComponent"
#define GET_COMPONENT_FUNCTION_NAME "GetComponent"
#define EMPLACE_OR_REPLACE_COMPONENT_FUNCTION_NAME "EmplaceOrReplaceComponent"

#define ADD_COMPONENT_FUNCTION() REFLECT_METHOD(ADD_COMPONENT_FUNCTION_NAME, &::BHive::GameObject::AddComponent<T>)
#define HAS_COMPONENT_FUNCTION() REFLECT_METHOD(HAS_COMPONENT_FUNCTION_NAME, &::BHive::GameObject::HasComponent<T>)
#define REMOVE_COMPONENT_FUNCTION() REFLECT_METHOD(REMOVE_COMPONENT_FUNCTION_NAME, &::BHive::GameObject::RemoveComponent<T>)
#define GET_COMPONENT_FUNCTION() REFLECT_METHOD(GET_COMPONENT_FUNCTION_NAME, &::BHive::GameObject::GetComponent<T>)
#define EMPLACE_OR_REPLACE_COMPONENT_FUNCTION() \
	REFLECT_METHOD(EMPLACE_OR_REPLACE_COMPONENT_FUNCTION_NAME, &::BHive::GameObject::EmplaceOrReplaceComponent<T>)

#define COMPONENT_IMPL()        \
	ADD_COMPONENT_FUNCTION()    \
	HAS_COMPONENT_FUNCTION()    \
	REMOVE_COMPONENT_FUNCTION() \
	GET_COMPONENT_FUNCTION()    \
	EMPLACE_OR_REPLACE_COMPONENT_FUNCTION()