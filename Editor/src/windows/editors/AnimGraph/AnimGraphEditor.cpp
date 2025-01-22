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

		OnGraphContextMenuEvent context_menu;
		context_menu.bind(this, &AnimGraphEditor::DrawCreateNodeMenu);

		OnGraphDragDropEvent drag_drop_event;
		drag_drop_event.bind(
			[=](const ImVec2 &p, const ImGuiPayload *payload)
			{
				auto type = *(rttr::type *)payload->Data;
				auto new_node = type.create().get_value<Ref<Node>>();
				mGraph.addNode(p, new_node);
			});

		mGraph.setGraphContext(context_menu);
		mGraph.setGraphDragDropEvent(drag_drop_event);
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
					auto new_node = type.create().get_value<Ref<Node>>();
					mGraph.addNode({}, new_node);
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
				auto new_node = type.create().get_value<Ref<Node>>();
				mGraph.addNode(pos, new_node);
			}
		}
	}

} // namespace BHive