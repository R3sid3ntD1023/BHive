#pragma once

#include "components/Components.h"
#include "GameObject.h"
#include "core/Application.h"
#include "core/Window.h"

namespace BHive
{
	struct Player : public GameObject
	{
		Player(const std::string &name, const entt::entity &handle, World *world)
			: GameObject(name, handle, world)
		{
			auto &window = Application::Get().GetWindow();
			auto &size = window.GetSize();
			auto &camera_component = AddComponent<CameraComponent>();
			camera_component.Camera = OrthographicCamera(10.f, size.x / (float)size.y);
		}
	};
} // namespace BHive