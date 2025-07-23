#pragma once

#include "inspectors/Inspect.h"
#include "mesh/SkeletalAnimation.h"
#include "asset/TAssetHandler.h"
#include "Animator/anim_graph/nodes/AnimGraphNodeBase.h"

namespace BHive
{
	class SkeletalAnimation;

	struct PoseNode : public AnimGraphNodeBase
	{
		PoseNode();

		REFLECTABLEV(AnimGraphNodeBase)
	};

	struct BlendNode : public PoseNode
	{
		struct PoseData
		{
			Ref<PoseNode> mNode;
			float mFactor{0.0f};
		};

		BlendNode();

		virtual void draw() override;

		REFLECTABLEV(PoseNode)

		std::vector<PoseData> mPoseDatas;
	};

	struct ClipNode : public PoseNode
	{
		ClipNode();

		virtual void draw() override;

		Ref<SkeletalAnimation> mAnimation;

		REFLECTABLEV(PoseNode)
	};

	struct PoseDataNode : public AnimGraphNodeBase
	{
		PoseDataNode();

		REFLECTABLEV(AnimGraphNodeBase)

	private:
		BlendNode::PoseData mPoseData;
	};

	struct StateTransition : public PoseNode
	{
		StateTransition();

		REFLECTABLEV(PoseNode)

	private:
		struct PoseNode *mCondition = nullptr;
		struct State *mDestination = nullptr;
	};

	struct State : public PoseNode
	{
		State();

		virtual void draw() override;

		REFLECTABLEV(PoseNode)

	private:
		struct PoseNode *mPoseNode = nullptr;
		struct std::vector<StateTransition *> mTransitions;
	};

	struct StateMachine : public PoseNode
	{
		StateMachine();

		REFLECTABLEV(PoseNode);
	};

	template <typename T>
	struct ArithmeticNode : public AnimGraphNodeBase
	{
		ArithmeticNode();

		virtual void draw();

		REFLECTABLEV(AnimGraphNodeBase)

	private:
		T mValue{};
	};

	template <typename T>
	inline ArithmeticNode<T>::ArithmeticNode()
	{
		addOUT<T>("Value")->behaviour([=]() { return mValue; });
	}

	template <typename T>
	inline void ArithmeticNode<T>::draw()
	{
		Inspect::inspect(typeid(T).name(), mValue, false, false, 100.0f);
	}

} // namespace BHive

#define REFLECT_ARITHMETIC_NODE(cls)                                      \
	REFLECT(ArithmeticNode<cls>)                                          \
	{                                                                     \
		BEGIN_REFLECT(ArithmeticNode<cls>, STRINGIFY(Arthmetic<##cls##>)) \
		REFLECT_CONSTRUCTOR();                                            \
	}