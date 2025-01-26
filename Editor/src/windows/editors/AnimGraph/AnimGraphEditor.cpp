#include "Animator/anim_graph/nodes/AnimGraphNodeBase.h"
#include "AnimGraphEditor.h"
#include "AnimGraphEditorNodes.h"
#include "gui/ImGuiExtended.h"

namespace BHive
{
	AnimGraphEditor::AnimGraphEditor()
	{
		auto derived = rttr::type::get<AnimGraphEditorNodeBase>().get_derived_classes();
		mDerivedNodes.insert(mDerivedNodes.end(), derived.begin(), derived.end());

		mGraph.rightClickPopUpContent(
			[=](ImFlow::BaseNode *hovered, const ImVec2 &pos)
			{
				if (!hovered)
				{
					DrawCreateNodeMenu(pos);
				}
				else
				{
					if (ImGui::MenuItem("Delete", "Delete"))
					{
						hovered->destroy();
					}
				}
			});
		mGraph.dragDropTarget(
			[=](const ImVec2 &pos)
			{
				auto payload = ImGui::AcceptDragDropPayload("NODE");
				if (payload)
				{
					auto type = (rttr::type *)payload->Data;
					auto new_node = type->create().get_value<Ref<ImFlow::BaseNode>>();
					mGraph.addNode(new_node, pos);
				}
			});
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
			for (auto &type : mDerivedNodes)
			{
				auto pretty_name = type.get_metadata("PrettyName");
				auto name = pretty_name ? pretty_name.to_string() : type.get_name();
				if (ImGui::Selectable(name.data()))
				{
					auto new_node = type.create().get_value<Ref<ImFlow::BaseNode>>();
					mGraph.addNode(new_node, {});
				}

				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("NODE", &type, sizeof(type));

					ImGui::EndDragDropSource();
				}
			}
		}

		ImGui::EndChild();
	}

	void AnimGraphEditor::DrawEditorWindow()
	{
		auto size = ImGui::GetContentRegionAvail();
		if (ImGui::BeginChild("Graph", size, ImGuiChildFlags_Border))
		{
			mGraph.update();
		}

		ImGui::EndChild();
	}

	void AnimGraphEditor::DrawCreateNodeMenu(const ImVec2 &pos)
	{
		for (auto &type : mDerivedNodes)
		{
			auto pretty_name = type.get_metadata("PrettyName");
			auto name = pretty_name ? pretty_name.to_string() : type.get_name();
			if (ImGui::Selectable(name.data()))
			{
				auto new_node = type.create().get_value<Ref<ImFlow::BaseNode>>();
				mGraph.addNode(new_node, pos);
			}
		}
	}

} // namespace BHive