#include "components/BoxComponent.h"
#include "components/CameraComponent.h"
#include "components/InputComponent.h"
#include "components/PhysicsComponent.h"
#include "components/SpriteComponent.h"
#include "components/TagComponent.h"
#include "core/Application.h"
#include "core/Window.h"
#include "Player.h"

namespace BHive
{
	Player::Player(World *world)
		: GameObject(world)
	{
		auto &window = Application::Get().GetWindow();
		auto &size = window.GetSize();
		auto &camera_component = AddComponent<CameraComponent>();
		camera_component.Camera.SetOrthographic(-10.f, 10.0f, -2.f, 18.0f, size.x / (float)size.y, -1.0f, 10.f);

		AddComponent<SpriteComponent>();
		AddComponent<BoxComponent>();
		AddComponent<InputComponent>();

		auto &physc = GetPhysicsComponent();
		physc.Settings.AngularLockAxis = AxisXYZ;
		physc.Settings.LinearLockAxis = AxisZ;
		physc.Settings.LinearDamping = 0.f;
		physc.Settings.Mass = 10.f;
		physc.Settings.BodyType = EBodyType::Dynamic;

		auto &bc = GetComponent<BoxComponent>();
		bc.mExtents = {.5f, .5f, .5f};
		bc.OnCollisionEnter.bind(this, &Player::OnCollisionEnter);

		Ref<InputContext> context;
		auto &input = GetComponent<InputComponent>();
		input.mInputContext = context = CreateRef<InputContext>();

		context->add_action("Jump", EKey::Space);
		context->add_action("MoveLeft", EKey::A);
		context->add_action("MoveRight", EKey::D);

		context->bind_key("Jump", EventStatus::PRESS, this, &Player::Jump);
		context->bind_axis("MoveLeft", this, &Player::Move, -1.f);
		context->bind_axis("MoveRight", this, &Player::Move, 1.f);
	}

	void Player::Jump(const InputValue &value)
	{
		LOG_TRACE("Jumped");
		auto &phys = GetPhysicsComponent();
		phys.SetBodyType(EBodyType::Dynamic);
		phys.ApplyForce({0, 6000.f, 0});
	}

	void Player::Move(const InputValue &value)
	{
		auto v = value.Get<float>();
		if (!IsNearlyZero(v) && !IsJumping())
		{
			auto &phys = GetPhysicsComponent();
			phys.SetBodyType(EBodyType::Dynamic);
			phys.ApplyForce({v * 100, 0, 0});
		}
	}

	bool Player::IsNearlyZero(float v, float threshold) const
	{
		return (v - threshold) <= 0 && (v + threshold) >= 0;
	}

	bool Player::IsJumping()
	{
		auto &phys = GetPhysicsComponent();
		auto y = phys.GetVelocity().y;
		return !IsNearlyZero(y, 0.1f);
	}

	void Player::OnCollisionEnter(ColliderComponent *component, GameObject *other)
	{
		if ((other->GetComponent<TagComponent>().Groups & BREAKABLE_BLOCKS) != 0)
		{
			other->Destroy();
			LOG_TRACE("Destroyed Block");
		}
	}
} // namespace BHive