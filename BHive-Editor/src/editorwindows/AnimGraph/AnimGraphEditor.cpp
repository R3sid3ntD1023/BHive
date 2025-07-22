#include "Animator/anim_graph/nodes/AnimGraphNodeBase.h"
#include "AnimGraphEditor.h"
#include "AnimGraphEditorNodes.h"
#include "gui/ImGuiExtended.h"
#include "AnimationGraph.h"

namespace BHive
{
	AnimGraphEditor::AnimGraphEditor()
	{
		mAnimationGraph = CreateRef<AnimationGraph>();
	}

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
			mAnimationGraph->GetCurrentGraph()->DrawCreateNodeMenu({});
		}

		ImGui::EndChild();
	}

	void AnimGraphEditor::DrawEditorWindow()
	{
		auto size = ImGui::GetContentRegionAvail();
		if (ImGui::BeginChild("Graph", size, ImGuiChildFlags_Border))
		{
			mAnimationGraph->GetCurrentGraph()->update();
		}

		ImGui::EndChild();
	}

} // namespace BHive