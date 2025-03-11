#pragma once

#include "core/Core.h"
#include "core/UUID.h"
#include "gfx/cameras/OrthographicCamera.h"
#include "math/Transform.h"
#include "sprite/Sprite.h"

namespace BHive
{
	struct TagComponent
	{
		TagComponent() = default;
		TagComponent(const TagComponent &) = default;
		TagComponent(const std::string &name)
			: Name(name)
		{
		}

		std::string Name;
	};

	struct TransformComponent
	{
		FTransform Transform;
	};

	struct SpriteComponent
	{
		glm::vec2 SpriteSize{1, 1};

		Color SpriteColor{0xffffffff};

		Ref<Sprite> Sprite;
	};

	struct CameraComponent
	{
		OrthographicCamera Camera;
		bool Primary = true;
	};

	struct IDComponent
	{
		UUID ID;
	};

	struct RelationshipComponent
	{
		UUID Parent = 0;
		std::unordered_set<UUID> Children;
	};
} // namespace BHive