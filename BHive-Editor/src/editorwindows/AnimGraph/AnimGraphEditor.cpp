#include "AnimGraphEditor.h"
#include "AnimGraphEditorNodes.h"
#include "AnimationGraph.h"
#include "Animator/anim_graph/nodes/AnimGraphNodeBase.h"
#include "gui/ImGuiExtended.h"

namespace BHive
{
	void AnimGraphEditor::OnUpdateContent()
	{

		DrawNodeList();

		ImGui::SameLine();

		DrawEditorWindow();

		TAssetEditor::OnUpdateContent();
	}

	void AnimGraphEditor::DrawNodeList()
	{
		auto size = ImGui::GetContentRegionAvail();
		if (ImGui::BeginChild("Nodes", {size.x * .25f, size.y}))
		{
			mAnimationGraph->DrawCreateNodeMenu({});
		}

		ImGui::EndChild();
	}

	void AnimGraphEditor::DrawEditorWindow()
	{
		auto size = ImGui::GetContentRegionAvail();
		if (ImGui::BeginChild("Graph", size, ImGuiChildFlags_Borders))
		{
			mAnimationGraph->update();
		}

		ImGui::EndChild();
	}

	void AnimGraphEditor::OnSetContext(const Ref<AnimGraph> &asset)
	{
		mAnimationGraph = CreateRef<AnimationGraph>(asset);
	}

} // namespace BHive