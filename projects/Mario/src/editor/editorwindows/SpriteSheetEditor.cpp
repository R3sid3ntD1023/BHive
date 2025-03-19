#include "SpriteSheetEditor.h"
#include "core/FileDialog.h"

namespace BHive
{
	void SpriteSheetEditor::OnWindowRender()
	{
		if (mAsset)
		{
			TAssetEditor::OnWindowRender();

			if (ImGui::BeginTable("##source", 2))
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				if (ImGui::BeginChild("Source"))
				{
					auto source = mAsset->GetSource();

					if (source)
					{
						auto size = ImGui::GetContentRegionAvail();
						auto ratio = (float)source->GetHeight() / (float)source->GetWidth();
						auto texture_size = size * ImVec2{1, ratio};
						ImGui::Image((ImTextureID)(uint64_t)(uint32_t)*source, texture_size, {0, 1}, {1, 0});
					}

					ImGui::EndChild();
				}

				ImGui::TableNextColumn();

				if (ImGui::BeginChild("Sprites"))
				{
					DrawSprites();

					ImGui::EndChild();
				}

				ImGui::EndTable();
			}

			ImGui::PushStyleColor(ImGuiCol_Button, {0, .6f, 0, 1});
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {.4f, .6f, .4f, 1});
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0, .6f, 0, 1});

			int index = 0;
			if (ImGui::Button("Extract"))
			{
				std::filesystem::path extract_path = FileDialogs::SaveFile("Sprite (*.sprite)\0*.sprite\0");
				if (!extract_path.empty())
				{
					auto file_name = extract_path.stem();
					auto parent_directory = extract_path.parent_path();
					auto ext = extract_path.extension();

					ExtractSprites(parent_directory, file_name.string(), ext.string());
					LOG_TRACE("Extractd Sprites");
				}
			}

			ImGui::PopStyleColor(3);
		}
	}

	void SpriteSheetEditor::DrawSprites()
	{
		auto &sprites = mAsset->GetSprites();
		auto &grid = mAsset->GetGrid();

		ImGui::TextDisabled("%llu", sprites.size());

		int i = 0;
		int columns = grid.mColumns > 0 ? grid.mColumns : 1;
		float avail_width = ImGui::GetContentRegionAvail().x;
		float item_width = avail_width / columns;
		if (ImGui::BeginTable("##sprites", columns, 0, {0, 0}, avail_width))
		{

			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			for (auto &sprite : sprites)
			{
				auto texture = sprite.GetSourceTexture();
				auto mincoords = sprite.GetMinCoords();
				auto maxcoords = sprite.GetMaxCoords();

				ImGui::BeginGroup();
				auto id = texture ? texture->GetRendererID() : 0;
				ImGui::Image(
					(ImTextureID)(uint64_t)(uint32_t)id, {item_width, item_width}, {mincoords.x, maxcoords.y},
					{maxcoords.x, mincoords.y});
				ImGui::TextColored({1, .5f, 0, 1}, "Sprite %d", i);
				ImGui::EndGroup();

				ImGui::TableNextColumn();
				i++;
			}

			ImGui::EndTable();
		}
	}

	void SpriteSheetEditor::ExtractSprites(
		const std::filesystem::path &directory, const std::string &filename, const std::string &ext)
	{
		auto manager = AssetManager::GetAssetManager<EditorAssetManager>();
		if (!manager)
			return;

		auto &sprites = mAsset->GetSprites();
		for (size_t i = 0; i < sprites.size(); i++)
		{
			auto &sprite = sprites[i];
			auto name = filename + std::to_string(i);
			auto export_path = directory / (name + ext);

			if (AssetFactory::Export(&sprite, export_path))
			{
				LOG_TRACE("Extracted Sprite {}", name);
			}

			manager->ImportAsset(export_path, sprite.get_type(), sprite.GetHandle());
		}
	}
} // namespace BHive
