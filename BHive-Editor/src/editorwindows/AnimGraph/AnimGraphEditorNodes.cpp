#include "AnimGraphEditorNodes.h"
#include "core/Casting.h"
#include "gui/ImGuiExtended.h"

namespace BHive
{
	namespace AnimEditor
	{
		template <typename T>
		std::function<bool(ImFlow::Pin *, ImFlow::Pin *)> IsSameNodeType()
		{
			return [](ImFlow::Pin *out, ImFlow::Pin *in)
			{
				auto a = ((NodeBase *)out->getParent())->get_type();
				auto b = rttr::type::get<T>();

				return a.is_derived_from(b) || a == b;
			};
		};

		PoseNode::PoseNode()
		{
			addOUT<PoseNode *>("", ImFlow::PinStyle::cyan())->behaviour([=]() { return this; });
		}

		BlendNode::BlendNode()
		{
			setTitle("Blend");

			setStyle(ImFlow::NodeStyle::green());
			addIN<float>("Factor", 0, ImFlow::ConnectionFilter::Numbers());
		}

		void BlendNode::draw()
		{
			for (size_t i = 0; i < mPoseDatas.size(); i++)
			{
				mPoseDatas[i] = showIN(std::format("Pose {}", i), PoseData{}, ImFlow::ConnectionFilter::SameType());
			}

			if (ImGui::Button("+"))
			{
				auto i = mPoseDatas.size();
				mPoseDatas.push_back(BlendNode::PoseData{});
			}

			if (mPoseDatas.size())
			{
				ImGui::SameLine();

				if (ImGui::Button("-"))
				{
					mPoseDatas.pop_back();
				}
			}
		}

		ClipNode::ClipNode()
		{
			setTitle("Clip");
			setStyle(ImFlow::NodeStyle::cyan());
		}

		void ClipNode::draw()
		{
			Inspect::inspect("Animation", mAnimation, false, false, 100.f);
		}

		PoseDataNode::PoseDataNode()
		{
			setTitle("PoseData");

			addIN<PoseNode *>("Node", &*mPoseData.mNode, IsSameNodeType<PoseNode>());
			addIN("Factor", mPoseData.mFactor, ImFlow::ConnectionFilter::Numbers());
			addOUT<BlendNode::PoseData>("PoseData")->behaviour([=]() { return mPoseData; });
		}

		State::State()
		{
			setTitle("State");
			addIN("Pose", mPoseNode, IsSameNodeType<PoseNode>());
		}

		void State::draw()
		{
			for (size_t i = 0; i < mTransitions.size(); i++)
			{
				mTransitions[i] =
					showIN<StateTransition *>(std::format("Transition {}", i), nullptr, IsSameNodeType<StateTransition>());
			}

			if (ImGui::Button("+"))
			{
				auto i = mTransitions.size();
				mTransitions.push_back(nullptr);
			}

			if (mTransitions.size())
			{
				ImGui::SameLine();

				if (ImGui::Button("-"))
				{
					mTransitions.pop_back();
				}
			}
		}

		StateMachine::StateMachine()
		{
		}

		StateTransition::StateTransition()
		{
			setTitle("Transition");
			addIN("Condition", mCondition, IsSameNodeType<PoseNode>());
			addIN("Destination", mDestination, IsSameNodeType<State>());
		}

	} // namespace AnimEditor

	REFLECT(AnimEditor::NodeBase)
	{
		BEGIN_REFLECT(AnimEditor::NodeBase);
	}

	REFLECT(AnimEditor::BlendNode)
	{
		BEGIN_REFLECT(AnimEditor::BlendNode, "Blend")
		REFLECT_CONSTRUCTOR();
	}

	REFLECT(AnimEditor::ClipNode)
	{
		BEGIN_REFLECT(AnimEditor::ClipNode, "Clip")
		REFLECT_CONSTRUCTOR();
	}

	REFLECT(AnimEditor::PoseDataNode)
	{
		BEGIN_REFLECT(AnimEditor::PoseDataNode, "Pose")
		REFLECT_CONSTRUCTOR();
	}

	REFLECT(AnimEditor::State)
	{
		BEGIN_REFLECT(AnimEditor::State, "State") REFLECT_CONSTRUCTOR();
	}

	REFLECT(AnimEditor::StateTransition)
	{
		BEGIN_REFLECT(AnimEditor::StateTransition, "StateTransition") REFLECT_CONSTRUCTOR();
	}

	REFLECT(AnimEditor::StateMachine)
	{
		BEGIN_REFLECT(AnimEditor::StateMachine, "StateMachine") REFLECT_CONSTRUCTOR();
	}

	REFLECT_ARITHMETIC_NODE(float)
	REFLECT_ARITHMETIC_NODE(int)
	REFLECT_ARITHMETIC_NODE(bool)
} // namespace BHive