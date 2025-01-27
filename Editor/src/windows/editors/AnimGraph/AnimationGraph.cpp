#include "AnimationGraph.h"
#include "AnimGraphEditorNodes.h"

namespace BHive
{

	AnimationGraph::AnimationGraph()
	{
		auto derived = GetSupportedNodeType().get_derived_classes();
		for (auto &type : derived)
		{
			if (type.get_constructor())
				mDerivedNodeTypes.push_back(type);
		}

		rightClickPopUpContent(
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
		dragDropTarget(
			[=](const ImVec2 &pos)
			{
				auto payload = ImGui::AcceptDragDropPayload("NODE");
				if (payload)
				{
					auto type = (rttr::type *)payload->Data;
					auto new_node = type->create().get_value<Ref<ImFlow::BaseNode>>();
					addNode(new_node, pos);
				}
			});

		mCurrentGraph = this;
	}

	void AnimationGraph::DrawCreateNodeMenu(const ImVec2 &pos)
	{
		for (auto &type : mDerivedNodeTypes)
		{
			auto pretty_name = type.get_metadata("PrettyName");
			auto name = pretty_name ? pretty_name.to_string() : type.get_name();
			if (ImGui::Selectable(name.data()))
			{
				auto new_node = type.create().get_value<Ref<ImFlow::BaseNode>>();
				addNode(new_node, pos);
			}

			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("NODE", &type, sizeof(type));

				ImGui::EndDragDropSource();
			}
		}
	}

	rttr::type AnimationGraph::GetSupportedNodeType() const
	{
		return rttr::type::get<AnimEditor::NodeBase>();
	}
} // namespace BHive