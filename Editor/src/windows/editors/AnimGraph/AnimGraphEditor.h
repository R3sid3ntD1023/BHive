#pragma once

#include "Animator/anim_graph/AnimGraph.h"
#include "windows/editors/TAssetEditor.h"

namespace BHive
{
	class AnimationGraph;

	class AnimGraphEditor : public TAssetEditor<AnimGraph>
	{
	public:
		AnimGraphEditor();

		virtual void OnWindowRender() override;

	private:
		void DrawNodeList();
		void DrawEditorWindow();

		Ref<AnimationGraph> mAnimationGraph;
	};

} // namespace BHive