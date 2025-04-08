#include "TextureEditor.h"
#include "asset/AssetFactory.h"
#include "asset/EditorAssetManager.h"
#include "core/FileDialog.h"

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

			// Creates subtexture from texture
			ImGui::SeparatorText("Create SubTexture");

			if (ImGui::BeginChild("Create SubTexture"))
			{
				inspect("X", mSubTexture.x);
				inspect("Y", mSubTexture.y);
				inspect("Width", mSubTexture.width);
				inspect("Height", mSubTexture.height);

				if (ImGui::Button("Create"))
				{
					auto path = FileDialogs::SaveFile(AssetFactory::GetFileFilters());
					if (!path.empty())
					{
						auto texture = mAsset->CreateSubTexture(mSubTexture);
						AssetFactory::Export(texture, path);
						AssetManager::GetAssetManager<EditorAssetManager>()->ImportAsset(
							path, texture->get_type(), texture->GetHandle());
					}
				}
				ImGui::EndChild();
			}

			ImGui::EndTable();
		}
	}
	void TextureEditor::OnSetContext(const Ref<Texture2D> &asset)
	{
		mSubTexture.width = mAsset->GetWidth();
		mSubTexture.height = mAsset->GetHeight();
	}
} // namespace BHive