#pragma once

#include "core/Core.h"
#include "components/Component.h"
#include "components/PhysicsComponent.h"
#include "core/UUID.h"
#include "math/Transform.h"
#include "World.h"
#include "core/serialization/Serialization.h"

namespace BHive
{

	struct GameObject
	{
		GameObject(World *world);

		virtual void Begin();
		virtual void Update(float dt);
		virtual void Render();
		virtual void End();

		template <typename T, typename... TArgs>
		T *AddComponent(TArgs &&...args)
		{
			ASSERT(!HasComponent<T>());

			auto component = CreateRef<T>(std::forward<TArgs>(args)...);
			component->mOwningObject = this;
			auto pair = mComponents.emplace(typeid(T).hash_code(), component);
			return component.get();
		}

		template <typename T>
		const T *GetComponent() const
		{
			ASSERT(HasComponent<T>());
			return Cast<T>(mComponents.at(typeid(T).hash_code())).get();
		}

		template <typename T>
		T *GetComponent()
		{
			ASSERT(HasComponent<T>());
			return Cast<T>(mComponents.at(typeid(T).hash_code())).get();
		}

		template <typename... T>
		auto GetComponents()
		{
			return mComponents;
		}

		template <typename T>
		void RemoveComponent()
		{
			ASSERT(HasComponent<T>());
			mComponents.erase(typeid(T).hash_code());
		}

		template <typename T>
		bool HasComponent() const
		{
			return mComponents.contains(typeid(T).hash_code());
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

		REFLECTABLEV()

	private:
		std::unordered_map<size_t, Ref<Component>> mComponents;
		PhysicsComponent mPhysicsComponent;
		World *mWorld = nullptr;
	};
} // namespace BHive

#define ADD_COMPONENT_FUNCTION_NAME "AddComponent"
#define REMOVE_COMPONENT_FUNCTION_NAME "RemoveComponent"
#define HAS_COMPONENT_FUNCTION_NAME "HasComponent"
#define GET_COMPONENT_FUNCTION_NAME "GetComponent"

#define ADD_COMPONENT_FUNCTION() REFLECT_METHOD(ADD_COMPONENT_FUNCTION_NAME, &::BHive::GameObject::AddComponent<T>)
#define HAS_COMPONENT_FUNCTION() REFLECT_METHOD(HAS_COMPONENT_FUNCTION_NAME, &::BHive::GameObject::HasComponent<T>)
#define REMOVE_COMPONENT_FUNCTION() REFLECT_METHOD(REMOVE_COMPONENT_FUNCTION_NAME, &::BHive::GameObject::RemoveComponent<T>)
#define GET_COMPONENT_FUNCTION() \
	REFLECT_METHOD(GET_COMPONENT_FUNCTION_NAME, rttr::select_overload<T *()>(&::BHive::GameObject::GetComponent<T>))
#define COMPONENT_IMPL()        \
	ADD_COMPONENT_FUNCTION()    \
	HAS_COMPONENT_FUNCTION()    \
	REMOVE_COMPONENT_FUNCTION() \
	GET_COMPONENT_FUNCTION()