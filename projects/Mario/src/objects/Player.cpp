#include "components/BoxComponent.h"
#include "components/CameraComponent.h"
#include "components/InputComponent.h"
#include "components/PhysicsComponent.h"
#include "components/SpriteComponent.h"
#include "core/Application.h"
#include "core/Window.h"
#include "Player.h"

namespace BHive
{
	Player::Player(const entt::entity &handle, World *world)
		: GameObject(handle, world)
	{
		auto &window = Application::Get().GetWindow();
		auto &size = window.GetSize();
		auto camera_component = AddComponent<CameraComponent>();
		camera_component->Camera.SetOrthographic(-10.f, 10.0f, -2.f, 18.0f, size.x / (float)size.y, -1.0f, 10.f);

		AddComponent<SpriteComponent>();
		AddComponent<BoxComponent>();
		AddComponent<InputComponent>();

		auto &physc = GetPhysicsComponent();
		physc.Settings.AngularLockAxis = AxisXYZ;
		physc.Settings.LinearLockAxis = AxisZ;
		physc.Settings.LinearDamping = 0.f;
		physc.Settings.Mass = 10.f;
		physc.Settings.BodyType = EBodyType::Dynamic;
	}

	void Player::Begin()
	{
		GameObject::Begin();

		auto input = GetComponent<InputComponent>();

		if (auto instance = input->GetInstance())
		{
			instance->bind_key("Jump", EventStatus::PRESS, this, &Player::Jump);
			instance->bind_axis("MoveLeft", this, &Player::Move, -1.f);
			instance->bind_axis("MoveRight", this, &Player::Move, 1.f);
		}

		auto bc = GetComponent<BoxComponent>();
		bc->OnCollisionEnter.bind(this, &Player::OnCollisionEnter);
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
		LOG_TRACE("CollisionEnter : {}", other->GetName());

		if (other->IsInGroup(BREAKABLE_BLOCKS))
		{
			other->Destroy();
			LOG_TRACE("Destroyed Block");
		}
	}

	REFLECT(Player)
	{
		BEGIN_REFLECT(Player)(META_DATA(ClassMetaData_Spawnable, true)) REFLECT_CONSTRUCTOR(const entt::entity &, World *);
	}
} // namespace BHive