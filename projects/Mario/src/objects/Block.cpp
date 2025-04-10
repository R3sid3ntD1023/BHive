#include "Block.h"
#include "components/BoxColliderComponent.h"
#include "components/FlipBookComponent.h"
#include "components/PhysicsComponent.h"
#include "components/SpriteComponent.h"
#include "GroupMacros.h"
#include "importers/TextureImporter.h"

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

} // namespace BHive