#pragma once

#include "core/Core.h"
#include "components/Component.h"
#include "components/PhysicsComponent.h"
#include "core/UUID.h"
#include "math/Transform.h"
#include "World.h"
#include "core/serialization/Serialization.h"
#include "core/EventDelegate.h"

namespace BHive
{
	DECLARE_EVENT(FOnDestroyed, GameObject *);

	struct FRegisteredComponent
	{
		size_t ComponentHash = 0;
		Component *ComponentPtr = nullptr;

		Component *operator->() const { return ComponentPtr; }

		Component &operator*() const { return *ComponentPtr; }
	};

	using ComponentList = std::vector<FRegisteredComponent>;

	struct GameObject
	{
		GameObject(const entt::entity &handle, World *world);
		GameObject(const GameObject &) = default;

		virtual void Begin();
		virtual void Update(float dt);
		virtual void Render();
		virtual void End();

		template <typename T, typename... TArgs>
		T *AddComponent(TArgs &&...args)
		{
			ASSERT(!HasComponent<T>());

			auto &component = mWorld->mRegistry.emplace<T>(mEntityHandle, std::forward<TArgs>(args)...);
			RegisterComponent<T>(&component);
			return &component;
		}

		template <typename T>
		T *EmplaceOrReplaceComponent(const Component &srcComponent)
		{
			auto &component = mWorld->mRegistry.emplace_or_replace<T>(mEntityHandle, static_cast<const T &>(srcComponent));
			RegisterComponent<T>(&component);
			return &component;
		}

		template <typename T>
		const T *GetComponent() const
		{
			ASSERT(HasComponent<T>());
			return &mWorld->mRegistry.get<T>(mEntityHandle);
		}

		template <typename T>
		T *GetComponent()
		{
			ASSERT(HasComponent<T>());
			return &mWorld->mRegistry.get<T>(mEntityHandle);
		}

		const ComponentList &GetComponents() const { return mComponents; }

		template <typename T>
		void RemoveComponent()
		{
			ASSERT(HasComponent<T>());
			UnRegisterComponent<T>();
			mWorld->mRegistry.remove<T>(mEntityHandle);
		}

		template <typename T>
		bool HasComponent() const
		{
			return mWorld->mRegistry.all_of<T>(mEntityHandle);
		}

		PhysicsComponent &GetPhysicsComponent();

		void SetName(const std::string &name);
		void SetTransform(const FTransform &transform);

		virtual void Save(cereal::BinaryOutputArchive &ar) const;
		virtual void Load(cereal::BinaryInputArchive &ar);

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

		operator entt::entity() const { return mEntityHandle; }

		FOnDestroyedEvent OnDestroyedEvent;

		REFLECTABLEV()

	private:
		template <typename T>
		size_t GetComponentHash() const
		{
			return typeid(T).hash_code();
		}

		template <typename T>
		void RegisterComponent(Component *component)
		{
			component->mOwningObject = this;
			auto hash = GetComponentHash<T>();

			auto it = std::find_if(
				mComponents.begin(), mComponents.end(),
				[hash](const FRegisteredComponent &comp) { return comp.ComponentHash == hash; });

			if (it != mComponents.end())
			{
				(*it).ComponentPtr = component;
				return;
			}

			mComponents.emplace_back(FRegisteredComponent{hash, component});
		}

		template <typename T>
		void UnRegisterComponent()
		{
			auto it = std::find_if(
				mComponents.begin(), mComponents.end(),
				[&](const FRegisteredComponent &comp) { return comp.ComponentHash == GetComponentHash<T>(); });
			if (it == mComponents.end())
				return;

			mComponents.erase(it);
		}

	private:
		entt::entity mEntityHandle{entt::null};
		ComponentList mComponents;
		World *mWorld = nullptr;
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
#define GET_COMPONENT_FUNCTION() \
	REFLECT_METHOD(GET_COMPONENT_FUNCTION_NAME, rttr::select_overload<T *()>(&::BHive::GameObject::GetComponent<T>))
#define EMPLACE_OR_REPLACE_COMPONENT_FUNCTION() \
	REFLECT_METHOD(EMPLACE_OR_REPLACE_COMPONENT_FUNCTION_NAME, &::BHive::GameObject::EmplaceOrReplaceComponent<T>)

#define COMPONENT_IMPL()        \
	ADD_COMPONENT_FUNCTION()    \
	HAS_COMPONENT_FUNCTION()    \
	REMOVE_COMPONENT_FUNCTION() \
	GET_COMPONENT_FUNCTION()    \
	EMPLACE_OR_REPLACE_COMPONENT_FUNCTION()