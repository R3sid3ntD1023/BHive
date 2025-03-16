#pragma once

#include "components/Component.h"
#include "components/PhysicsComponent.h"
#include "core/UUID.h"
#include "math/Transform.h"
#include "World.h"

namespace BHive
{

	struct GameObject
	{
		GameObject(World *world);

		virtual void Begin();
		virtual void Update(float dt);
		virtual void End();

		template <typename T, typename... TArgs>
		T &AddComponent(TArgs &&...args)
		{
			ASSERT(!HasComponent<T>());

			auto component = CreateRef<T>(std::forward<TArgs>(args)...);
			component->mOwningObject = this;
			auto pair = mComponents.emplace(typeid(T).hash_code(), component);
			return *component;
		}

		template <typename T>
		const T &GetComponent() const
		{
			ASSERT(HasComponent<T>());
			return static_cast<T &>(*mComponents.at(typeid(T).hash_code()));
		}

		template <typename T>
		T &GetComponent()
		{
			ASSERT(HasComponent<T>());
			return static_cast<T &>(*mComponents.at(typeid(T).hash_code()));
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

	private:
		std::unordered_map<size_t, Ref<Component>> mComponents;
		PhysicsComponent mPhysicsComponent;
		World *mWorld = nullptr;
	};
} // namespace BHive