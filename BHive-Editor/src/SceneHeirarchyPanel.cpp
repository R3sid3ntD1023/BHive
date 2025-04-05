#include "core/subsystem/SubSystem.h"
#include "gui/GUICore.h"
#include "SceneHeirarchyPanel.h"
#include "subsystems/SelectionSubSystem.h"
#include "world/GameObject.h"
#include "world/World.h"

namespace BHive
{
#define DRAG_DROP_GAMEOBJECT "GAMEOBJECT"

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<World> &world)
		: mWorld(world)
	{
	}

	void SceneHierarchyPanel::OnGuiRender()
	{
		auto &selection = GetSubSystem<SelectionSubSystem>();

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

		if (ImGui::BeginPopupContextWindow(
				"SceneHierarchyPanel", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonDefault_))
		{

			if (ImGui::MenuItem("Create Entity"))
			{
				auto new_obj = mWorld->CreateGameObject("New Object");
				selection.Select(new_obj.get());
			}

			auto &entity_types = GetSpawnableEntites();
			for (auto &type : entity_types)
			{
				if (ImGui::MenuItem(type.get_name().data()))
				{
					auto world = mWorld.get();
					auto obj = type.create({world->CreateEntity(), world}).get_value<Ref<GameObject>>();
					mWorld->AddGameObject(obj);
					selection.Select(obj.get());
				}
			}

			ImGui::EndPopup();
		}

		ImGui::Dummy(ImGui::GetContentRegionAvail());

		if (ImGui::BeginDragDropTarget())
		{
			if (auto payload = ImGui::AcceptDragDropPayload(DRAG_DROP_GAMEOBJECT))
			{
				auto id = *(UUID *)payload->Data;
				mWorld->GetGameObject(id)->RemoveParent();
			}

			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
		{
			selection.Clear();
		}
	}

	void SceneHierarchyPanel::SetContext(const Ref<World> &world)
	{
		mWorld = world;
	}

	void SceneHierarchyPanel::DrawNode(GameObject *obj)
	{
		auto &selection = GetSubSystem<SelectionSubSystem>();

		bool destroyed = false;

		ImGuiTreeNodeFlags flags =
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

		bool selected = selection.GetSelection() == obj;

		flags |= (selected ? ImGuiTreeNodeFlags_Selected : 0);

		ImGui::PushID(obj);
		bool opened = ImGui::TreeNodeEx(obj->GetName().c_str(), flags);

		if (ImGui::IsItemClicked())
		{
			if (!selected)
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
				destroyed = true;
			}
			ImGui::EndPopup();
		}

		if (selected)
		{
			if ((ImGui::IsKeyDown(ImGuiKey_ModAlt) && ImGui::IsKeyPressed(ImGuiKey_P)))
			{
				obj->RemoveParent();
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_Delete))
			{
				destroyed = true;
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

		if (destroyed)
		{
			if (selection.GetSelection() == obj)
				selection.Clear();
			obj->Destroy();
		}
	}

	const std::vector<AssetType> &SceneHierarchyPanel::GetSpawnableEntites()
	{
		auto &cache = mEntityTypeCache;
		if (cache.size())
			return cache;

		auto derived = AssetType::get<GameObject>().get_derived_classes();
		for (auto &type : derived)
		{
			if (type.get_metadata(ClassMetaData_Spawnable))
			{
				cache.push_back(type);
			}
		}

		return cache;
	}
} // namespace BHive