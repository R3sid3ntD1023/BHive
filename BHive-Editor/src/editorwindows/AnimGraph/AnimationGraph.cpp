#include "AnimationGraph.h"
#include "AnimGraphEditorNodes.h"
#include "Animator/anim_graph/AnimGraph.h"

namespace BHive
{

	AnimationGraph::AnimationGraph(const Ref<AnimGraph> &graph)
		: mGraph(graph)
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
					auto new_node = type->create().get_value<Ref<AnimGraphNodeBase>>();
					addNode(new_node, pos);
					if (mGraph)
						mGraph->AddNode(new_node);
				}
			});
	}

	void AnimationGraph::DrawCreateNodeMenu(const ImVec2 &pos)
	{
		for (auto &type : mDerivedNodeTypes)
		{
			auto pretty_name = type.get_metadata("PrettyName");
			auto name = pretty_name ? pretty_name.to_string() : type.get_name();
			if (ImGui::Selectable(name.data()))
			{
				auto new_node = type.create().get_value<Ref<AnimGraphNodeBase>>();
				addNode(new_node, pos);

				if (mGraph)
					mGraph->AddNode(new_node);
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
		return rttr::type::get<AnimGraphNodeBase>();
	}
} // namespace BHive