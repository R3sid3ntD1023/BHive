#include "Animator/anim_graph/nodes/AnimGraphNodeBase.h"
#include "AnimGraphEditor.h"
#include "gui/ImGuiExtended.h"

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
			auto derived_types = rttr::type::get<AnimGraphNodeBase>().get_derived_classes();
			for (auto &type : derived_types)
			{
				if (ImGui::MenuItem(type.get_name().data()))
				{
				}
			}
		}

		ImGui::EndChild();
	}

	void AnimGraphEditor::DrawEditorWindow()
	{
		auto size = ImGui::GetContentRegionAvail();
		if (ImGui::BeginChild("Graph", size))
		{
		}

		ImGui::EndChild();
	}

} // namespace BHive