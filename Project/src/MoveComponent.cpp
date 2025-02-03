#include "MoveComponent.h"
#include "scene/components/InputComponent.h"
#include "scene/components/FlipbookComponent.h"
#include "scene/Entity.h"
#include "IInputActor.h"

namespace BHive
{
	void MoveComponent::OnBegin()
	{
		auto entity = GetOwner();

		if (auto input_entity = Cast<IInputActor>(entity))
		{
			if (auto context = input_entity->GetInputComponent()->GetContext())
			{
				context->bind_key("Jump", BHive::EventStatus::RELEASE, this, &MoveComponent::Jump);
				context->bind_axis("MoveLeft", this, &MoveComponent::Move, -1.f);
				context->bind_axis("MoveRight", this, &MoveComponent::Move, 1.f);
			}
		}
	}

	void MoveComponent::Jump(const BHive::InputValue &value)
	{
		// auto &rigid_body_component = GetComponent<BHive::RigidBodyComponent>();
		// rigid_body_component.ApplyForce({0, 300, 0});
	}

	void MoveComponent::Move(const BHive::InputValue &value)
	{
		auto direction = value.Get<float>() * mSpeed;

		// auto &rigid_body_component = GetComponent<BHive::RigidBodyComponent>();
		// rigid_body_component.ApplyForce({direction, 0, 0});

		UpdateState();
	}

	void MoveComponent::UpdateState()
	{
		// auto &rigid_body_component = GetComponent<BHive::RigidBodyComponent>();
		// auto &flipbook_component = GetComponent<BHive::FlipbookComponent>();
	}

	void MoveComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		Component::Save(ar);
		ar(mSpeed, mJumpForce, mStates);
	}

	void MoveComponent::Load(cereal::BinaryInputArchive &ar)
	{
		Component::Load(ar);
		ar(mSpeed, mJumpForce, mStates);
	}

	REFLECT(MoveComponent)
	{
		BEGIN_REFLECT(MoveComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS() REFLECT_PROPERTY("Speed", mSpeed)
			REFLECT_PROPERTY("JumpForce", mJumpForce) REFLECT_PROPERTY("States", mStates);
	}
} // namespace BHive