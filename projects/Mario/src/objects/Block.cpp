#include "Block.h"
#include "GroupMacros.h"
#include "importers/TextureImporter.h"
#include "runtime/components/BoxColliderComponent.h"
#include "runtime/components/FlipBookComponent.h"
#include "runtime/components/PhysicsComponent.h"
#include "runtime/components/SpriteComponent.h"

namespace BHive
{
	BlockBase::BlockBase(const entt::entity &handle, World *world)
		: GameObject(handle, world)
	{
		AddComponent<BoxColliderComponent>();
		AddComponent<PhysicsComponent>();

		auto physc = GetPhysicsComponent();
		physc->Settings.AngularLockAxis = AxisXYZ;
		physc->Settings.LinearLockAxis = AxisZ;
		physc->Settings.BodyType = EBodyType::Static;
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

	void QuestionBlock::OnCollisionEnter(ColliderComponent *component, GameObject *other)
	{
		/*if ((other->GetTag() & PLAYER) != 0)
		{
			Destroy();
			GetWorld()->SpawnGameObject(SpawnedBlock, GetTransform(), "New Item");
		}*/
	}

	RTTR_REGISTRATION
	{
		{
			BEGIN_REFLECT(BlockBase);
		}
		{
			BEGIN_REFLECT(Block)
			REFLECT_CONSTRUCTOR(const entt::entity &, World *);
		}

		{
			BEGIN_REFLECT(QuestionBlock)
			REFLECT_CONSTRUCTOR(const entt::entity &, World *);
			// REFLECT_PROPERTY("BlockToSpawn", SpawnedBlock);
		}
	}

} // namespace BHive