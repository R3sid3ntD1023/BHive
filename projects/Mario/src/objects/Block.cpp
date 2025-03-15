#include "Block.h"
#include "components/BoxComponent.h"
#include "components/PhysicsComponent.h"

namespace BHive
{
	Block::Block(const entt::entity &handle, World *world)
		: GameObject(handle, world)

	{
		AddComponent<SpriteComponent>();
		AddComponent<PhysicsComponent>();
		AddComponent<BoxComponent>();

		auto [physc, bc, sc] = GetComponents<PhysicsComponent, BoxComponent, SpriteComponent>();
		physc.mAngularLockAxis = AxisXYZ;
		physc.mLinearLockAxis = AxisZ;
		physc.mBodyType = EBodyType::Static;
	}

	void Block::SetSize(const glm::vec2 &size)
	{
		GetComponent<SpriteComponent>().SpriteSize = size;
		GetComponent<BoxComponent>().mExtents = {size.x * .5f, size.y * .5f, 1.0f};
	}

	void Block::SetTiling(const glm::vec2 &tiling)
	{
		GetComponent<SpriteComponent>().Tiling = tiling;
	}

	void Block::SetSprite(const Ref<Sprite> &sprite)
	{
		GetComponent<SpriteComponent>().Sprite = sprite;
	}
} // namespace BHive