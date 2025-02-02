#include "PrefabEditor.h"

namespace BHive
{
	PrefabEditor::PrefabEditor()
	{
		mSceneHierarchy.mOnObjectSelected.bind([=](ObjectBase *obj, bool append) { mSelection.Select(obj, append); });
		mSceneHierarchy.mOnObjectDeselected.bind([=](ObjectBase *obj) { mSelection.Deselect(obj); });
		mSceneHierarchy.mOnGetActiveObject.bind([=]() { return mSelection.GetActiveObject(); });
		mSceneHierarchy.mIsObjectSelected.bind([=](ObjectBase *obj) { return mSelection.IsSelected(obj); });
		mSceneHierarchy.mClearSelection.bind([=]() { mSelection.Clear(); });

		mProperties.mGetActiveObject.bind([=]() { return mSelection.GetActiveObject(); });
		mProperties.mOnObjectSelected.bind([=](ObjectBase *obj, bool append) { mSelection.Select(obj, append); });
		mProperties.mOnObjectDeselected.bind([=](ObjectBase *obj) { mSelection.Deselect(obj); });
		mProperties.mGetActiveEntity.bind([=]() { return mSelection.GetActiveEntity(); });

		mDetails.mGetActiveObject.bind([=]() { return mSelection.GetActiveObject(); });
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