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
			auto image_size = ImGui::GetContentRegionAvail().x * .5f;
			auto texture = mAsset->GetSourceTexture();

			if (texture)
			{

				ImGui::Image(
					(ImTextureID)(uint64_t)(uint32_t)*texture, {image_size, image_size},
					{min.x, max.y}, {max.x, min.y});
				ImGui::SameLine();
			}

			if (ImGui::BeginChild("Variables", {image_size, image_size}, ImGuiChildFlags_AlwaysUseWindowPadding))
			{
				TAssetEditor::OnWindowRender();
			}

			ImGui::EndChild();

			if (texture)
			{
				auto size = ImGui::GetContentRegionAvail();
				min.x *= size.x;
				min.y *= size.y;
				max.x *= size.x;
				max.y *= size.y;
				min.y = size.y - min.y;
				max.y = size.y - max.y;

				ImGui::Image(
					(ImTextureID)(uint64_t)(uint32_t)*texture, {size.x, size.y}, {0, 1}, {1, 0});
				auto item_pos = ImGui::GetItemRectMin();

				ImGui::SetCursorScreenPos(item_pos);

				ImRect bounds = {ImVec2(min.x, min.y), ImVec2(max.x, max.y)};
				if (ImGui::EditableRect("##spritebounds", &bounds.Min.x, &bounds.Max.x, 30.0f))
				{

					ImRect newbounds = {bounds.Min, bounds.Max};
					newbounds.Min.y = size.y - newbounds.Min.y;
					newbounds.Max.y = size.y - newbounds.Max.y;

					auto newmin =
						(glm::vec2(newbounds.Min.x, newbounds.Min.y) / glm::vec2(size.x, size.y));
					auto newmax =
						(glm::vec2(newbounds.Max.x, newbounds.Max.y) / glm::vec2(size.x, size.y));

					mAsset->SetCoords(newmin, newmax);
				}
			}
		}
	}

	void SpriteSheetEditor::OnWindowRender()
	{
		if (mAsset)
		{
			TAssetEditor::OnWindowRender();

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
						(ImTextureID)(uint64_t)(uint32_t)id, {item_width, item_width},
						{mincoords.x, maxcoords.y}, {maxcoords.x, mincoords.y});
					ImGui::TextColored({1, .5f, 0, 1}, "Sprite %d", i);
					ImGui::EndGroup();

					ImGui::TableNextColumn();
					i++;
				}

				ImGui::EndTable();
			}

			ImGui::PushStyleColor(ImGuiCol_Button, {0, .6f, 0, 1});
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {.4f, .6f, .4f, 1});
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0, .6f, 0, 1});

			int index = 0;
			if (ImGui::Button("Extract"))
			{
				std::filesystem::path extract_path =
					FileDialogs::SaveFile("Sprite (*.sprite)\0*.sprite\0");
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

			AssetFactory factory;
			if (factory.Export(&sprite, export_path))
			{
				LOG_TRACE("Extracted Sprite {}", name);
			}

			manager->ImportAsset(export_path, sprite.get_type(), sprite.GetHandle());
		}
	}
} // namespace BHive