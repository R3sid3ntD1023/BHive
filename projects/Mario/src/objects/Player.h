#pragma once

#include "GameObject.h"

#define BREAKABLE_BLOCKS 0x01

namespace BHive
{
	struct InputValue;

	struct Player : public GameObject
	{
		Player(World *world);

		void Jump(const InputValue &value);

		void Move(const InputValue &value);

		bool IsNearlyZero(float v, float threshold = 0.001f) const;

		bool IsJumping();

	private:
		void OnCollisionEnter(struct ColliderComponent *component, GameObject *other);
	};
} // namespace BHive