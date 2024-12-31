#include "SceneHeirarchyPanel.h"
#include "scene/World.h"
#include "scene/Actor.h"
#include "gui/GUICore.h"
#include "subsystem/SubSystem.h"
#include "subsystems/EditSubSystem.h"

#define DRAG_DROP_ACTOR_NAME "ACTOR"

namespace BHive
{

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<World> &world)
		: mWorld(world)
	{
	}

	void SceneHierarchyPanel::OnGuiRender()
	{
		mDestroyedActors.clear();

		auto &edit_subsystem = SubSystemContext::Get().GetSubSystem<EditSubSystem>();

		if (mWorld)
		{
			auto &actors = mWorld->GetActors();
			for (auto &[id, actor] : actors)
			{
				if (actor->GetParent())
					continue;

				DrawActorNode(actor.get());
			}

			for (auto actor : mDestroyedActors)
				actor->Destroy();
		}

		if (ImGui::BeginPopupContextWindow("SceneHierarchyPanel", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonDefault_))
		{

			if (ImGui::MenuItem("Create Actor"))
			{
				auto new_actor = mWorld->CreateActor();
				edit_subsystem.mSelection.Select(new_actor.get());
			}

			auto &actor_types = GetSpawnableActors();
			for (auto &type : actor_types)
			{
				if (ImGui::MenuItem(type.get_name().data()))
				{
					auto new_actor = type.create().get_value<Ref<Actor>>();
					mWorld->AddActor(new_actor);
					edit_subsystem.mSelection.Select(new_actor.get());
				}
			}

			ImGui::EndPopup();
		}

		ImGui::Dummy(ImGui::GetContentRegionAvail());

		if (ImGui::BeginDragDropTarget())
		{
			if (auto payload = ImGui::AcceptDragDropPayload(DRAG_DROP_ACTOR_NAME))
			{
				auto actor = *(Actor **)payload->Data;
				actor->DetachFromParent();
			}

			ImGui::EndDragDropTarget();
		}
	}

	void SceneHierarchyPanel::SetContext(const Ref<World> &world)
	{
		mWorld = world;
	}

	void SceneHierarchyPanel::DrawActorNode(Actor *actor)
	{
		auto &edit_subsystem = SubSystemContext::Get().GetSubSystem<EditSubSystem>();

		bool destroyed = false;

		static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth |
										  ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

		auto id = (uint64_t)actor->GetUUID();

		ImGui::PushID(id);
		bool opened = ImGui::TreeNodeEx(actor->GetName().c_str(), flags);

		if (ImGui::IsItemClicked() || ImGui::IsItemFocused())
		{
			edit_subsystem.mSelection.Select(actor);
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload(DRAG_DROP_ACTOR_NAME, &actor, sizeof(Actor));

			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (auto payload = ImGui::AcceptDragDropPayload(DRAG_DROP_ACTOR_NAME))
			{
				auto other_actor = *(Actor **)payload->Data;
				if (other_actor != actor)
				{
					other_actor->AttachTo(actor);
				}
			}

			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (actor->GetParent())
			{
				if (ImGui::MenuItem("Remove From Parent", "Alt + P"))
				{
					actor->DetachFromParent();
				}
			}

			if (ImGui::MenuItem("Duplicate", "Ctrl + D"))
			{
			}
			if (ImGui::MenuItem("Delete", "Delete"))
			{
				destroyed = true;
			}
			ImGui::EndPopup();
		}

		if (opened)
		{
			for (auto child : actor->GetChildren())
			{
				DrawActorNode(child);
			}
			ImGui::TreePop();
		}

		ImGui::PopID();

		if (destroyed)
		{
			edit_subsystem.mSelection.Deselect(actor, DeselectReason_Destroyed);
			mDestroyedActors.push_back(actor);
		}
	}

	const std::vector<AssetType> &SceneHierarchyPanel::GetSpawnableActors()
	{
		auto &cache = mActorTypeCache;
		if (cache.size())
			return cache;

		auto derived = AssetType::get<Actor>().get_derived_classes();
		for (auto &type : derived)
		{
			if (type.get_metadata(ClassMetaData_ActorSpawnable))
			{
				cache.push_back(type);
			}
		}

		return cache;
	}
}