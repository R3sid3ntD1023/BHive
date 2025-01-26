#pragma once

#include "inspector/Inspectors.h"
#include "mesh/SkeletalAnimation.h"
#include <asset/TAssetHandler.h>
#include <ImNodeFlow.h>
#include <reflection/Reflection.h>

namespace BHive
{
	struct AnimGraphEditorNodeBase : public ImFlow::BaseNode
	{
		REFLECTABLEV()
	};

	struct AnimGraphBlendNode : public AnimGraphEditorNodeBase
	{
		struct PoseData
		{
			Ref<AnimGraphEditorNodeBase> mNode;
			float mFactor{0.0f};
		};

		AnimGraphBlendNode();

		virtual void draw() override;

		REFLECTABLEV(AnimGraphEditorNodeBase)

		std::vector<PoseData> mPoseDatas;
	};

	class SkeletalAnimation;

	struct AnimGraphClipNode : public AnimGraphEditorNodeBase
	{
		AnimGraphClipNode();

		virtual void draw() override;

		TAssetHandle<SkeletalAnimation> mAnimation;

		REFLECTABLEV(AnimGraphEditorNodeBase)
	};

	struct AnimGraphPoseDataNode : public AnimGraphEditorNodeBase
	{
		AnimGraphPoseDataNode();

		REFLECTABLEV(AnimGraphEditorNodeBase)

	private:
		AnimGraphBlendNode::PoseData mPoseData;
	};

	template <typename T>
	struct ArithmeticNode : public AnimGraphEditorNodeBase
	{
		ArithmeticNode();

		virtual void draw();

		REFLECTABLEV(AnimGraphEditorNodeBase)

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

#define REFLECT_ARITHMETIC_NODE(cls)       \
	REFLECT(ArithmeticNode<cls>)           \
	{                                      \
		BEGIN_REFLECT(ArithmeticNode<cls>) \
		REFLECT_CONSTRUCTOR();             \
	}
} // namespace BHive
