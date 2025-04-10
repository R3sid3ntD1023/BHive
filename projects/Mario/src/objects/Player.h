#pragma once

#include "GameObject.h"

#define BREAKABLE_BLOCKS 0x01

namespace BHive
{
	struct InputValue;

	struct Player : public GameObject
	{
		Player(const entt::entity &handle, World *world);

		void Begin() override;

		void Jump(const InputValue &value);

		void Move(const InputValue &value);

		bool IsNearlyZero(float v, float threshold = 0.001f) const;

		bool IsJumping();

		REFLECTABLEV(GameObject)

	private:
		void OnCollisionEnter(struct ColliderComponent *component, GameObject *other);
	};
} // namespace BHive