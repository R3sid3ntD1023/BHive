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

	void Block::SetSize(const glm::vec2 &size)
	{
		GetComponent<SpriteComponent>().SpriteSize = size;
		GetComponent<BoxComponent>().Extents = {size.x * .5f, size.y * .5f, 1.0f};
	}

	void Block::SetTiling(const glm::vec2 &tiling)
	{
		GetComponent<SpriteComponent>().Tiling = tiling;
	}

	void Block::SetSprite(const Ref<Sprite> &sprite)
	{
		GetComponent<SpriteComponent>().Sprite = sprite;
	}

	QuestionBlock::QuestionBlock(World *world)
		: BlockBase(world)
	{
		auto &fb_component = AddComponent<FlipBookComponent>();

		auto texture = TextureLoader::Import(RESOURCE_PATH "textures/items.png");
		Ref<Sprite> sprites[3] = {};

		sprites[0] = Sprite::Create(texture, FSpriteGenerator{{0, 4}, {16, 16}, {1, 1}});
		sprites[1] = Sprite::Create(texture, FSpriteGenerator{{1, 4}, {16, 16}, {1, 1}});
		sprites[2] = Sprite::Create(texture, FSpriteGenerator{{2, 4}, {16, 16}, {1, 1}});

		auto flipbook = CreateRef<FlipBook>();
		flipbook->AddFrame(sprites[0], 5);
		flipbook->AddFrame(sprites[1], 5);
		flipbook->AddFrame(sprites[2], 5);
		flipbook->SetLoop(true);

		fb_component.FlipBook = flipbook;
	}

} // namespace BHive