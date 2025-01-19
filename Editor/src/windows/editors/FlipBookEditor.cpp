#include "FlipBookEditor.h"
#include "gfx/Texture.h"

namespace BHive
{
	void FlipBookEditor::OnWindowRender()
	{
		if (mAsset)
		{

			auto length = mAsset->GetTotalTime();

			auto &frames = mAsset->GetFrames();
			auto count = (uint32_t)frames.size();

			auto width = ImGui::GetContentRegionAvail().x * .5f;

			if (ImGui::BeginChild("Data", {width, 0}, ImGuiChildFlags_ResizeX | ImGuiChildFlags_AlwaysUseWindowPadding))
			{
				TAssetEditor::OnWindowRender();
			}

			ImGui::EndChild();

			ImGui::SameLine();

			if (ImGui::BeginChild("CurrentFrame", {0, 0}, ImGuiChildFlags_AlwaysUseWindowPadding))
			{

				if (count)
				{
					int32_t texture_id = 0;
					glm::vec2 min = {0, 1}, max = {1, 0};

					auto sprite = mAsset->GetSpriteAtFrame(mCurrentFrame);
					if (sprite)
					{
						auto texture = sprite->GetSourceTexture();
						texture_id = texture ? texture->GetRendererID() : 0;

						min = sprite->GetMinCoords();
						max = sprite->GetMaxCoords();
					}

					float item_width = ImGui::GetContentRegionAvail().x;
					ImGui::Image((ImTextureID)(uint64_t)(uint32_t)texture_id, {item_width, item_width}, {min.x, max.y}, {max.x, min.y});

					ImGui::Timeline("##timeline", &mCurrentFrame, (int)frames.size());
				}
			}

			ImGui::EndChild();
		}
	}
}