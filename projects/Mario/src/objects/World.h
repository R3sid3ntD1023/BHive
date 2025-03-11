#pragma once

#include "core/Core.h"
#include <entt/entt.hpp>

namespace BHive
{
	class GameObject;
	class Texture2D;

	class World
	{
	public:
		World(const std::string &name);

		void Update(float dt);
		void Resize(uint32_t w, uint32_t h);

		template <typename T>
		Ref<T> CreateGameObject(const std::string &name)
			requires(std::is_base_of_v<GameObject, T>)
		{
			auto object = CreateRef<T>(name, mRegistry.create(), this);
			AddGameObject(object);
			return object;
		}

		void AddGameObject(const Ref<GameObject> &object);

		Ref<GameObject> GetGameObject(const UUID &id) const;

		const std::string &GetName() const { return mName; }

	private:
		std::string mName;
		std::unordered_map<UUID, Ref<GameObject>> mObjects;
		entt::registry mRegistry;

		friend class GameObject;
	};
} // namespace BHive