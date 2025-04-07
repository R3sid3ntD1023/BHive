#include "Block.h"
#include "components/BoxComponent.h"
#include "components/PhysicsComponent.h"
#include "components/SpriteComponent.h"
#include "components/FlipBookComponent.h"
#include "importers/TextureImporter.h"

namespace BHive
{
	BlockBase::BlockBase(World *world)
		: GameObject(world)
	{
		AddComponent<BoxComponent>();

		auto &physc = GetPhysicsComponent();
		physc.Settings.AngularLockAxis = AxisXYZ;
		physc.Settings.LinearLockAxis = AxisZ;
		physc.Settings.BodyType = EBodyType::Static;
	}

	Block::Block(World *world)
		: BlockBase(world)

	{
		AddComponent<SpriteComponent>();
	}

	QuestionBlock::QuestionBlock(World *world)
		: BlockBase(world)
	{
		AddComponent<FlipBookComponent>();
	}

	REFLECT(BlockBase)
	{
		BEGIN_REFLECT(BlockBase);
	}

	REFLECT(Block)
	{
		BEGIN_REFLECT(Block)(META_DATA(ClassMetaData_Spawnable, true)) REFLECT_CONSTRUCTOR(World *);
	}

	REFLECT(QuestionBlock)
	{
		BEGIN_REFLECT(QuestionBlock)(META_DATA(ClassMetaData_Spawnable, true)) REFLECT_CONSTRUCTOR(World *);
	}
} // namespace BHive