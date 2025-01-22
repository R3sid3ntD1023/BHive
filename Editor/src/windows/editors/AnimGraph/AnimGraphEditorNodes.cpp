#include "AnimGraphEditorNodes.h"
#include "core/Casting.h"

namespace BHive
{
	AnimGraphBlendNode::AnimGraphBlendNode()
	{
		setTitle("Blend");
		/*setStyle(ImFlow::NodeStyle::green());
		addIN<float>("Factor", 0, ImFlow::ConnectionFilter::Numbers());*/
	}

	void AnimGraphBlendNode::draw()
	{
		for (size_t i{}; i < mPoseDatas.size(); i++)
		{
			// showIN(std::format("Pose{}", i), mPoseDatas[i], ImFlow::ConnectionFilter::SameType());
		}

		if (ImGui::Button("+", {20.f, 0.f}))
		{
			mPoseDatas.push_back({});
		}
	}

	AnimGraphClipNode::AnimGraphClipNode()
	{
		setTitle("Clip");
		/*setStyle(ImFlow::NodeStyle::cyan());
		addOUT<AnimGraphEditorNodeBase *>("Animation", ImFlow::PinStyle::cyan())->behaviour([=]() { return this; });*/
	}

	void AnimGraphClipNode::draw()
	{
		inspect("Animation", mAnimation, false, false, meta_data_empty, 200.f);
	}

	AnimGraphPoseDataNode::AnimGraphPoseDataNode()
	{
		setTitle("PoseData");

		/*	addIN<AnimGraphEditorNodeBase *>("Node", mPoseData.mNode.get(), ImFlow::ConnectionFilter::SameType());
			addIN("Factor", mPoseData.mFactor, ImFlow::ConnectionFilter::Numbers());
			addOUT<AnimGraphBlendNode::PoseData>("PoseData")->behaviour([=]() { return mPoseData; });*/
	}

	REFLECT(AnimGraphEditorNodeBase)
	{
		BEGIN_REFLECT(AnimGraphEditorNodeBase);
	}

	REFLECT(AnimGraphBlendNode)
	{
		BEGIN_REFLECT(AnimGraphBlendNode)
		REFLECT_CONSTRUCTOR();
	}

	REFLECT(AnimGraphClipNode)
	{
		BEGIN_REFLECT(AnimGraphClipNode)
		REFLECT_CONSTRUCTOR();
	}

	REFLECT_ARITHMETIC_NODE(float)
	REFLECT_ARITHMETIC_NODE(int)
	REFLECT_ARITHMETIC_NODE(bool)
} // namespace BHive