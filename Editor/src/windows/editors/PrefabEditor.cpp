#include "PrefabEditor.h"

namespace BHive
{
	PrefabEditor::PrefabEditor()
	{
		mSceneHierarchy.mOnEntitySelected.bind([=](ObjectBase *obj) { mSelection.Select(obj); });
		mSceneHierarchy.mOnEntityDeselected.bind([=](ObjectBase *obj, uint8_t reason) { mSelection.Deselect(obj, (EDeselectReason)reason); });
		mSceneHierarchy.mOnGetSelectedObject.bind([=]() { return mSelection.GetSelectedObject(); });
		mProperties.mGetSelectedEntity.bind([=]() { return mSelection.GetSelectedEntity(); });
		mProperties.mGetSelectedObject.bind([=]() { return mSelection.GetSelectedObject(); });
		mProperties.mOnObjectSelected.bind([=](ObjectBase *obj) { mSelection.Select(obj); });
		mProperties.mOnObjectDeselected.bind([=](ObjectBase *obj, uint8_t reason) { mSelection.Deselect(obj, (EDeselectReason)reason); });
		mDetails.mGetSelectedObject.bind([=]() { return mSelection.GetSelectedObject(); });
	}

	void PrefabEditor::OnWindowRender()
	{
		bool add_child_entity = false;

		auto size = ImGui::GetContentRegionAvail();

		if (ImGui::BeginTable("Windows", 2, ImGuiTableFlags_Resizable, size))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			if (auto prefab = Cast<Prefab>(mAsset))
			{
				auto world = prefab->GetInstance();

				if (ImGui::BeginChild("#Root", {}, ImGuiChildFlags_AlwaysUseWindowPadding))
				{
					mSceneHierarchy.OnGuiRender();
				}

				ImGui::EndChild();

				ImGui::TableNextColumn();

				if (ImGui::BeginChild("Properties", {0, size.y * .5f}, ImGuiChildFlags_AlwaysUseWindowPadding))
				{
					mProperties.OnGuiRender();
				}

				ImGui::EndChild();

				if (ImGui::BeginChild("Details", {}, ImGuiChildFlags_AlwaysUseWindowPadding))
				{
					mDetails.OnGuiRender();
				}

				ImGui::EndChild();
			}

			ImGui::EndTable();
		}
	}

	void PrefabEditor::OnSetContext(const Ref<Asset> &asset)
	{
		if (auto prefab = Cast<Prefab>(asset))
		{
			mSceneHierarchy.SetContext(prefab->GetInstance());
		}
	}
} // namespace BHive