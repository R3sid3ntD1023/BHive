#include "PrefabEditor.h"

namespace BHive
{
	void PrefabEditor::OnWindowRender()
	{
		bool add_child_entity = false;

		auto size = ImGui::GetContentRegionAvail();

		if (ImGui::BeginTable("Windows", 2, ImGuiTableFlags_Resizable, size))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			auto world = mAsset->GetInstance();

			if (ImGui::BeginChild("#Root", {}, ImGuiChildFlags_AlwaysUseWindowPadding))
			{
				mSceneHierarchy.OnGuiRender();
			}

			ImGui::EndChild();

			ImGui::TableNextColumn();

			ImGui::EndTable();
		}
	}

	void PrefabEditor::OnSetContext(const Ref<Prefab> &asset)
	{
		mSceneHierarchy.SetContext(asset->GetInstance());
	}
} // namespace BHive