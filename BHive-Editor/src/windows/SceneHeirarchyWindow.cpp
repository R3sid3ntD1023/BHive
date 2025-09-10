#include "SceneHeirarchyWindow.h"
#include "core/subsystem/SubSystem.h"
#include "gui/GUICore.h"
#include "world/GameObject.h"
#include "world/World.h"
#include "subsystems/Selection.h"
#include "inspectors/Inspect.h"
#include "core/threading/Threading.h"

namespace BHive
{
#define DRAG_DROP_GAMEOBJECT "GAMEOBJECT"

	ImSceneHierarchy::ImSceneHierarchy(const Ref<World> &world)
		: mWorld(world)
	{
	}

	void ImSceneHierarchy::OnUpdateContent()
	{
		auto &selection = GetSubSystem<Selection>();

		if (ImGui::BeginChild("##GameObjects", {}, ImGuiChildFlags_ResizeY | ImGuiChildFlags_AlwaysUseWindowPadding))

		{
			if (mWorld)
			{
				auto &objs = mWorld->GetGameObjects();
				for (auto &[id, obj] : objs)
				{
					if (obj->GetParent())
						continue;

					DrawNode(obj.get());
				}
			}
		}

		if (ImGui::BeginPopupContextWindow("ImSceneHierarchy", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonDefault_))
		{

			if (ImGui::MenuItem("Add New GameObject"))
			{
				auto new_obj = mWorld->CreateGameObject("NewGameObject");
				selection.Select(new_obj.get());
			}

			auto derived = rttr::type::get<GameObject>().get_derived_classes();
			for (auto &type : derived)
			{
				if (ImGui::MenuItem(type.get_name().data()))
				{
					auto world = mWorld.get();
					auto obj = world->CreateGameObject(type);
					selection.Select(obj.get());
				}
			}

			ImGui::EndPopup();
		}

		if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
		{
			selection.Clear();
		}

		ImGui::EndChild();

		if (ImGui::BeginDragDropTarget())
		{
			if (auto payload = ImGui::AcceptDragDropPayload(DRAG_DROP_GAMEOBJECT))
			{
				auto id = *(UUID *)payload->Data;
				mWorld->GetGameObject(id)->RemoveFromParent();
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::SeparatorText("Properties");

		if (ImGui::BeginChild("##Properties", {}, ImGuiChildFlags_AlwaysUseWindowPadding))
		{
			auto gameobject = selection.GetSelection();

			if (gameobject)
				Inspect::get().inspect("", mWorld.get(), gameobject);
		}

		ImGui::EndChild();
	}

	void ImSceneHierarchy::SetContext(const Ref<World> &world)
	{
		mWorld = world;
	}

	void ImSceneHierarchy::DrawNode(GameObject *obj)
	{
		auto &selection = GetSubSystem<Selection>();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

		bool selected = selection.IsSelected(obj);

		flags |= (selected ? ImGuiTreeNodeFlags_Selected : 0);
		flags |= (!obj->HasChildren() ? ImGuiTreeNodeFlags_Leaf : 0);

		ImGui::PushID(obj);
		bool opened = ImGui::TreeNodeEx(obj->GetName().c_str(), flags);

		if (ImGui::IsItemClicked())
		{
			selection.Select(obj);
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload(DRAG_DROP_GAMEOBJECT, &obj->GetID(), sizeof(UUID));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (auto payload = ImGui::AcceptDragDropPayload(DRAG_DROP_GAMEOBJECT))
			{
				auto id = *(UUID *)payload->Data;
				obj->AddChild(mWorld->GetGameObject(id).get());
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete", "Delete"))
			{
				Thread::Dispatch(
					[obj, &selection]()
					{
						if (selection.GetSelection() == obj)
							selection.Clear();

						obj->Destroy();
					});
			}

			if (ImGui::MenuItem("Duplicate", "Ctr + D"))
			{
				Thread::Dispatch([=]() { mWorld->DuplicateGameobject(obj); });
			}

			ImGui::EndPopup();
		}

		bool focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
		if (selected && focused)
		{
			if ((ImGui::IsKeyDown(ImGuiKey_ModAlt) && ImGui::IsKeyPressed(ImGuiKey_P)))
			{
				obj->RemoveFromParent();
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_Delete))
			{
				Thread::Dispatch(
					[obj, &selection]()
					{
						if (selection.GetSelection() == obj)
							selection.Clear();

						obj->Destroy();
					});
			}
		}

		if (opened)
		{
			for (auto &child : obj->GetChildren())
			{
				DrawNode(child.get());
			}
			ImGui::TreePop();
		}

		ImGui::PopID();
	}

} // namespace BHive