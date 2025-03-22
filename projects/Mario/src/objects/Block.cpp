#include "Block.h"
#include "components/BoxComponent.h"
#include "components/PhysicsComponent.h"
#include "components/SpriteComponent.h"
#include "components/FlipBookComponent.h"
#include "importers/TextureImporter.h"

namespace BHive
{
	BlockBase::BlockBase(const entt::entity &handle, World *world)
		: GameObject(handle, world)
	{
		AddComponent<BoxComponent>();

		auto &physc = GetPhysicsComponent();
		physc.Settings.AngularLockAxis = AxisXYZ;
		physc.Settings.LinearLockAxis = AxisZ;
		physc.Settings.BodyType = EBodyType::Static;
	}

	Block::Block(const entt::entity &handle, World *world)
		: BlockBase(handle, world)

	{
		AddComponent<SpriteComponent>();
	}

	QuestionBlock::QuestionBlock(const entt::entity &handle, World *world)
		: BlockBase(handle, world)
	{
		AddComponent<FlipBookComponent>();
	}

	REFLECT(BlockBase)
	{
		BEGIN_REFLECT(BlockBase);
	}

	REFLECT(Block)
	{
		BEGIN_REFLECT(Block)(META_DATA(ClassMetaData_Spawnable, true)) REFLECT_CONSTRUCTOR(const entt::entity &, World *);
	}

	REFLECT(QuestionBlock)
	{
		BEGIN_REFLECT(QuestionBlock)(META_DATA(ClassMetaData_Spawnable, true))
			REFLECT_CONSTRUCTOR(const entt::entity &, World *);
	}
} // namespace BHive