#include "FlipBookEditor.h"
#include "gfx/Texture.h"
#include "gfx/imgui/IImGuiProvider.h"
#include "gfx/sprite/Sprite.h"

namespace BHive
{
	void FlipBookEditor::OnUpdateContent()
	{
		if (mAsset)
		{

			auto length = mAsset->GetTotalTime();

			auto &frames = mAsset->GetFrames();
			auto count = (uint32_t)frames.size();

			auto width = ImGui::GetContentRegionAvail().x * .5f;

			if (ImGui::BeginChild("Data", {width, 0}, ImGuiChildFlags_ResizeX | ImGuiChildFlags_AlwaysUseWindowPadding))
			{
				TAssetEditor::OnUpdateContent();
			}

			ImGui::EndChild();

			ImGui::SameLine();

			if (ImGui::BeginChild("CurrentFrame", {0, 0}, ImGuiChildFlags_AlwaysUseWindowPadding))
			{

				if (count)
				{
					glm::vec2 min = {0, 1}, max = {1, 0};

					auto spriteHandle = mAsset->GetSpriteAtFrame(mCurrentFrame);
					if (spriteHandle)
					{
						auto sprite = spriteHandle.As<Sprite>();
						auto textureHandle = sprite->GetSourceTexture();

						min = sprite->GetMinCoords();
						max = sprite->GetMaxCoords();

						if (textureHandle)
						{
							float item_width = ImGui::GetContentRegionAvail().x;
							auto id = IImGuiTextureProvider::GetID(*textureHandle.As<Texture>());
							ImGui::Image(id, {item_width, item_width}, {min.x, max.y}, {max.x, min.y});
						}
					}

					if (ImGui::Timeline("##timeline", &mCurrentFrame, (int)frames.size()))
					{
						LOG_TRACE("Current Frame: {}", mCurrentFrame);
					}
				}
			}

			ImGui::EndChild();
		}
	}
} // namespace BHive