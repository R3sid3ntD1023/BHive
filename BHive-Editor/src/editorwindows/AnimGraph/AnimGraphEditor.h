#pragma once

#include "editorwindows/TAssetEditor.h"
#include "runtime/animation/anim_graph/AnimGraph.h"

namespace BHive
{
	class AnimationGraph;

	class AnimGraphEditor : public TAssetEditor<AnimGraph>
	{
	public:
		AnimGraphEditor() = default;

		virtual void OnUpdateContent() override;

		virtual void OnSetContext(const Ref<AnimGraph> &asset);

	private:
		void DrawNodeList();
		void DrawEditorWindow();

		Ref<AnimationGraph> mAnimationGraph;
	};

} // namespace BHive