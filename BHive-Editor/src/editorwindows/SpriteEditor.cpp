#include "asset/EditorAssetManager.h"
#include "core/FileDialog.h"
#include "gfx/Texture.h"
#include "SpriteEditor.h"

namespace BHive
{
	void SpriteEditor::OnWindowRender()
	{
		if (mAsset)
		{
			auto min = mAsset->GetMinCoords();
			auto max = mAsset->GetMaxCoords();
			auto texture = mAsset->GetSourceTexture();

			if (ImGui::BeginChild("##Texture", {}, ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_ResizeY))
			{

				TAssetEditor::OnWindowRender();

				if (texture)
				{
					auto max_size = ImGui::GetContentRegionAvail().y;
					auto texture_size = ImVec2{max_size, max_size / texture->GetAspectRatio()};
					ImGui::Image((ImTextureID)(uint64_t)(uint32_t)*texture, texture_size, {min.x, max.y}, {max.x, min.y});
				}
			}

			ImGui::EndChild();

			if (ImGui::BeginChild(
					"##Coordinates", {0, 200.f}, ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_ResizeY))
			{
				if (texture)
				{
					auto size = ImGui::GetContentRegionAvail();

					ImGui::Image((ImTextureID)(uint64_t)(uint32_t)*texture, {size.x, size.y}, {0, 1}, {1, 0});
					auto item_pos = ImGui::GetItemRectMin();

					ImGui::SetCursorScreenPos(item_pos);

					ImRect bounds = {
						ImVec2(min.x * size.x, size.y - (min.y * size.y)),
						ImVec2(max.x * size.x, size.y - (max.y * size.y))};
					if (ImGui::EditableRect("##spritebounds", &bounds.Min.x, &bounds.Max.x, 10.0f))
					{

						ImRect newbounds = {bounds.Min, bounds.Max};
						newbounds.Min.y = size.y - newbounds.Min.y;
						newbounds.Max.y = size.y - newbounds.Max.y;

						auto newmin = (glm::vec2(newbounds.Min.x, newbounds.Min.y) / glm::vec2(size.x, size.y));
						auto newmax = (glm::vec2(newbounds.Max.x, newbounds.Max.y) / glm::vec2(size.x, size.y));

						mAsset->SetCoords(newmin, newmax);
					}
				}
			}

			ImGui::EndChild();
		}
	}

} // namespace BHive