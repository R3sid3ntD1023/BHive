#include "SceneHeirarchyPanel.h"
#include "objects/World.h"
#include "objects/GameObject.h"
#include "gui/GUICore.h"

namespace BHive
{

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<World> &world)
		: mWorld(world)
	{
	}

	void SceneHierarchyPanel::OnGuiRender()
	{
		mDestroyedObjects.clear();

		if (mWorld)
		{
			auto &objs = mWorld->GetGameObjects();
			for (auto &[id, obj] : objs)
			{
				if (obj->GetParent())
					continue;

				DrawNode(obj.get());
			}

			for (auto obj : mDestroyedObjects)
				obj->Destroy();
		}

		if (ImGui::BeginPopupContextWindow(
				"SceneHierarchyPanel", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonDefault_))
		{

			if (ImGui::MenuItem("Create Entity"))
			{
				auto new_obj = mWorld->CreateGameObject("New Object");
			}

			auto &entity_types = GetSpawnableEntites();
			for (auto &type : entity_types)
			{
				if (ImGui::MenuItem(type.get_name().data()))
				{
					auto obj = type.create().get_value<Ref<GameObject>>();
					mWorld->AddGameObject(obj);
				}
			}

			ImGui::EndPopup();
		}

		ImGui::Dummy(ImGui::GetContentRegionAvail());

		// if (ImGui::BeginDragDropTarget())
		// {
		// 	if (auto payload = ImGui::AcceptDragDropPayload(DRAG_DROP_ENTITY_NAME))
		// 	{
		// 		auto entity = *(Entity **)payload->Data;
		// 		entity->DetachFromParent();
		// 	}

		// 	ImGui::EndDragDropTarget();
		// }

		// if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
		// {
		// 	if (mClearSelection)
		// 	{
		// 		mClearSelection();
		// 	}
		// }
	}

	void SceneHierarchyPanel::SetContext(const Ref<World> &world)
	{
		mWorld = world;
	}

	void SceneHierarchyPanel::DrawNode(GameObject *obj)
	{
		bool destroyed = false;

		ImGuiTreeNodeFlags flags =
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

		auto id = (uint64_t)obj->GetID();
		bool selected = mSelectedObject == obj;

		flags |= (selected ? ImGuiTreeNodeFlags_Selected : 0);

		ImGui::PushID(id);
		bool opened = ImGui::TreeNodeEx(obj->GetName().c_str(), flags);

		if (ImGui::IsItemClicked())
		{
			bool append = ImGui::IsKeyDown(ImGuiKey_ModShift);
			bool ctrl_down = ImGui::IsKeyDown(ImGuiKey_ModCtrl);

			if (ctrl_down)
			{
				mSelectedObject = nullptr;
			}
			else
			{
				mSelectedObject = obj;
			}
		}

		if (ImGui::BeginDragDropSource())
		{

			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{

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
			mSelectedObject = nullptr;
			mDestroyedObjects.push_back(obj);
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