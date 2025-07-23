#include "Animator/anim_graph/nodes/AnimGraphNodeBase.h"
#include "AnimGraphEditor.h"
#include "AnimGraphEditorNodes.h"
#include "gui/ImGuiExtended.h"
#include "AnimationGraph.h"

namespace BHive
{
	void AnimGraphEditor::OnWindowRender()
	{

		DrawNodeList();

		ImGui::SameLine();

		DrawEditorWindow();

		TAssetEditor::OnWindowRender();
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
		if (ImGui::BeginChild("Graph", size, ImGuiChildFlags_Border))
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