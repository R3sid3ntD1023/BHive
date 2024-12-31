#include "MoveComponent.h"
#include "scene/components/InputComponent.h"
#include "scene/components/FlipbookComponent.h"
#include "scene/Actor.h"
#include "IInputActor.h"

namespace BHive
{
    void MoveComponent::OnBegin()
    {
        auto actor = GetOwningActor();

        if (auto input_actor = Cast<IInputActor>(actor))
        {
            if (auto context = input_actor->GetInputComponent()->GetContext())
            {
                context->bind_key("Jump", BHive::InputAction::RELEASE, this, &MoveComponent::Jump);
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

    void MoveComponent::Serialize(StreamWriter &ar) const
    {
        ar(mSpeed, mJumpForce, mStates);
    }

    void MoveComponent::Deserialize(StreamReader &ar)
    {
        ar(mSpeed, mJumpForce, mStates);
    }

    REFLECT(MoveComponent)
    {
        BEGIN_REFLECT(MoveComponent)
        (META_DATA(ClassMetaData_ComponentSpawnable, true))
            REQUIRED_COMPONENT_FUNCS()
                REFLECT_PROPERTY("Speed", mSpeed)
                    REFLECT_PROPERTY("JumpForce", mJumpForce)
                        REFLECT_PROPERTY("States", mStates);
    }
}