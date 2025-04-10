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

	using ComponentList = std::vector<Component *>;

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
			ASSERT(!HasComponent<T>())
			auto component = &mWorld->mRegistry.emplace<T>(mEntity, std::forward<TArgs>(args)...);
			AddComponent(component);
			return component;
		}

		template <typename T>
		T *EmplaceOrReplaceComponent(Component *component)
		{
			auto new_component = &mWorld->mRegistry.emplace_or_replace<T>(mEntity, *dynamic_cast<T *>(component));
			AddComponent(new_component);
			return new_component;
		}

		template <typename T>
		void RemoveComponent()
		{
			ASSERT(HasComponent<T>())

			T *component = GetComponent<T>();
			auto it = std::find_if(
				mComponents.begin(), mComponents.end(), [component](Component *comp) { return component == comp; });

			mComponents.erase(it);
			mWorld->mRegistry.remove<T>(mEntity);
		}

		template <typename T>
		T *GetComponent()
		{
			return &mWorld->mRegistry.get<T>(mEntity);
		}

		template <typename T>
		const T *GetComponent() const
		{
			return &mWorld->mRegistry.get<T>(mEntity);
		}

		template <typename T>
		bool HasComponent() const
		{
			return mWorld->mRegistry.any_of<T>(mEntity);
		}

		void SetName(const std::string &name);
		void SetLocalTransform(const FTransform &transform);
		void SetWorldTransform(const FTransform &transform);

		PhysicsComponent *GetPhysicsComponent();

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
		FTransform GetWorldTransform() const;
		FTransform &GetLocalTransform();
		const FTransform &GetLocalTransform() const;
		const uint64_t GetGroup() const;
		bool IsInGroup(uint16_t group) const;

		World *GetWorld() const { return mWorld; }
		Ref<GameObject> GetParent() const;
		std::unordered_set<Ref<GameObject>> GetChildren() const;
		const ComponentList &GetComponents() const { return mComponents; }
		ComponentList &GetComponents() { return mComponents; }

		FOnDestroyedEvent OnDestroyedEvent;

		operator entt::entity() const { return mEntity; }

		REFLECTABLEV()

	private:
		void AddComponent(Component *component);
		Component *GetOrAddComponent(const rttr::type &type);

	protected:
		World *mWorld = nullptr;

		ComponentList mComponents;

		entt::entity mEntity{entt::null};
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
	REFLECT_METHOD(GET_COMPONENT_FUNCTION_NAME, rttr::select_const(&::BHive::GameObject::GetComponent<T>))
#define EMPLACE_OR_REPLACE_COMPONENT_FUNCTION() \
	REFLECT_METHOD(EMPLACE_OR_REPLACE_COMPONENT_FUNCTION_NAME, &::BHive::GameObject::EmplaceOrReplaceComponent<T>)

#define COMPONENT_IMPL()        \
	ADD_COMPONENT_FUNCTION()    \
	HAS_COMPONENT_FUNCTION()    \
	REMOVE_COMPONENT_FUNCTION() \
	GET_COMPONENT_FUNCTION()    \
	EMPLACE_OR_REPLACE_COMPONENT_FUNCTION()