#include "SceneHeirarchyPanel.h"
#include "scene/World.h"
#include "scene/Entity.h"
#include "gui/GUICore.h"
#include "subsystem/SubSystem.h"
#include "subsystems/EditSubSystem.h"

#define DRAG_DROP_entity_NAME "entity"

namespace BHive
{

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<World> &world)
		: mWorld(world)
	{
	}

	void SceneHierarchyPanel::OnGuiRender()
	{
		mDestroyedentitys.clear();

		auto &edit_subsystem = SubSystemContext::Get().GetSubSystem<EditSubSystem>();

		if (mWorld)
		{
			auto &entities = mWorld->GetEntities();
			for (auto &[id, entity] : entities)
			{
				if (entity->GetParent())
					continue;

				DrawEntityNode(entity.get());
			}

			for (auto entity : mDestroyedentitys)
				entity->Destroy();
		}

		if (ImGui::BeginPopupContextWindow("SceneHierarchyPanel", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonDefault_))
		{

			if (ImGui::MenuItem("Create Entity"))
			{
				auto new_entity = mWorld->CreateEntity();
				edit_subsystem.mSelection.Select(new_entity.get());
			}

			auto &entity_types = GetSpawnableEntites();
			for (auto &type : entity_types)
			{
				if (ImGui::MenuItem(type.get_name().data()))
				{
					auto new_entity = type.create().get_value<Ref<Entity>>();
					mWorld->AddEntity(new_entity);
					edit_subsystem.mSelection.Select(new_entity.get());
				}
			}

			ImGui::EndPopup();
		}

		ImGui::Dummy(ImGui::GetContentRegionAvail());

		if (ImGui::BeginDragDropTarget())
		{
			if (auto payload = ImGui::AcceptDragDropPayload(DRAG_DROP_entity_NAME))
			{
				auto entity = *(Entity **)payload->Data;
				entity->DetachFromParent();
			}

			ImGui::EndDragDropTarget();
		}
	}

	void SceneHierarchyPanel::SetContext(const Ref<World> &world)
	{
		mWorld = world;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity *entity)
	{
		auto &edit_subsystem = SubSystemContext::Get().GetSubSystem<EditSubSystem>();

		bool destroyed = false;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth |
										  ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

		auto id = (uint64_t)entity->GetUUID();
		bool selected = edit_subsystem.mSelection.GetSelectedObject() == entity;

		flags |= (selected ? ImGuiTreeNodeFlags_Selected: 0);

		ImGui::PushID(id);
		bool opened = ImGui::TreeNodeEx(entity->GetName().c_str(), flags);
		

		if (ImGui::IsItemClicked() || ImGui::IsItemFocused())
		{
			edit_subsystem.mSelection.Select(entity);
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload(DRAG_DROP_entity_NAME, &entity, sizeof(Entity));

			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (auto payload = ImGui::AcceptDragDropPayload(DRAG_DROP_entity_NAME))
			{
				auto other_entity = *(Entity **)payload->Data;
				if (other_entity != entity)
				{
					other_entity->AttachTo(entity);
				}
			}

			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (entity->GetParent())
			{
				if (ImGui::MenuItem("Remove From Parent", "Alt + P"))
				{
					entity->DetachFromParent();
				}
			}

			
			if (ImGui::MenuItem("Duplicate", "Ctrl + D"))
			{
				mWorld->DuplicateEntity(entity);
			}

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
				entity->DetachFromParent();
			}
			else if ((ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_D)))
			{
				mWorld->DuplicateEntity(entity);
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_Delete))
			{
				destroyed = true;
			}
		}

		if (opened)
		{
			for (auto child : entity->GetChildren())
			{
				DrawEntityNode(child);
			}
			ImGui::TreePop();
		}

		ImGui::PopID();

		if (destroyed)
		{
			edit_subsystem.mSelection.Deselect(entity, DeselectReason_Destroyed);
			mDestroyedentitys.push_back(entity);
		}
	}

	const std::vector<AssetType> &SceneHierarchyPanel::GetSpawnableEntites()
	{
		auto &cache = mentityTypeCache;
		if (cache.size())
			return cache;

		auto derived = AssetType::get<Entity>().get_derived_classes();
		for (auto &type : derived)
		{
			if (type.get_metadata(ClassMetaData_Spawnable))
			{
				cache.push_back(type);
			}
		}

		return cache;
	}
}