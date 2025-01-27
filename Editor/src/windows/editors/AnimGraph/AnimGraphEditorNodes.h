#pragma once

#include "inspector/Inspectors.h"
#include "mesh/SkeletalAnimation.h"
#include "asset/TAssetHandler.h"
#include "reflection/Reflection.h"
#include <ImNodeFlow.h>

namespace BHive
{
	class SkeletalAnimation;

	namespace AnimEditor
	{
		struct NodeBase : public ImFlow::BaseNode
		{
			REFLECTABLEV()
		};

		struct PoseNode : public NodeBase
		{
			PoseNode();

			REFLECTABLEV(NodeBase)
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

			TAssetHandle<SkeletalAnimation> mAnimation;

			REFLECTABLEV(PoseNode)
		};

		struct PoseDataNode : public NodeBase
		{
			PoseDataNode();

			REFLECTABLEV(NodeBase)

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
		struct ArithmeticNode : public NodeBase
		{
			ArithmeticNode();

			virtual void draw();

			REFLECTABLEV(NodeBase)

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
			inspect(typeid(T).name(), mValue, false, false, meta_data_empty, 100.f);
		}
	} // namespace AnimEditor

} // namespace BHive

#define REFLECT_ARITHMETIC_NODE(cls)                         \
	REFLECT(AnimEditor::ArithmeticNode<cls>)                 \
	{                                                        \
		BEGIN_REFLECT(AnimEditor::ArithmeticNode<cls>, #cls) \
		REFLECT_CONSTRUCTOR();                               \
	}