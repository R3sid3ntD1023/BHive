#include "Block.h"
#include "components/BoxComponent.h"
#include "components/PhysicsComponent.h"
#include "components/SpriteComponent.h"

namespace BHive
{
	Block::Block(World *world)
		: GameObject(world)

	{
		AddComponent<SpriteComponent>();
		AddComponent<BoxComponent>();

		auto &physc = GetPhysicsComponent();
		physc.Settings.AngularLockAxis = AxisXYZ;
		physc.Settings.LinearLockAxis = AxisZ;
		physc.Settings.BodyType = EBodyType::Static;
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