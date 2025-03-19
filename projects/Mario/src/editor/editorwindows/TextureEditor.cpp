#include "TextureEditor.h"

namespace BHive
{
	void TextureEditor::OnWindowRender()
	{
		if (mAsset)
		{
			ImGui::BeginTable("##Table", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable);
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			if (ImGui::BeginChild("##Image"))
			{
				auto size = ImGui::GetContentRegionAvail();
				auto ratio = (float)mAsset->GetHeight() / (float)mAsset->GetWidth();
				auto texture_size = size * ImVec2{1, ratio};

				ImGui::SetCursorPos((size - texture_size) * .5f);
				ImGui::Image((ImTextureID)(uint64_t)(uint32_t)*mAsset, texture_size, {0, 1}, {1, 0});

				ImGui::EndChild();
			}

			ImGui::TableNextColumn();

			TAssetEditor::OnWindowRender();

			ImGui::EndTable();
		}
	}
} // namespace BHive