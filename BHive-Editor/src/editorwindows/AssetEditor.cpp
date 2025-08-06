#include "AssetEditor.h"
#include "core/FileDialog.h"

namespace BHive
{
	AssetEditor::AssetEditor(int flags)
		: ImWindowBase(ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_HorizontalScrollbar | flags)
	{
	}

	void AssetEditor::OnMenuBar()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save", "Ctrl + S"))
			{
				Save();
			}

			if (ImGui::MenuItem("SaveAs", "Ctrl +Alt + S"))
			{
				SaveAs();
			}

			ImGui::EndMenu();
		}

		if (IsFocused())
		{
			if (ImGui::IsKeyPressed(ImGuiKey_S))
			{
				if (ImGui::IsKeyDown(ImGuiKey_ModCtrl))
				{

					if (ImGui::IsKeyDown(ImGuiKey_ModAlt))
					{
						SaveAs();
					}
					else
					{
						Save();
					}
				}
			}
		}
	}

	void AssetEditor::SaveAs()
	{
		auto path = FileDialogs::SaveFile("Asset (*.asset)\0*.asset");
		if (!path.empty())
		{
			mCurrentSavePath = path;
			Save();
		}
	}

	void AssetEditor::Save()
	{
		OnSave(mCurrentSavePath);
		LOG_TRACE("Saved Asset {} at", mCurrentSavePath);
	}

} // namespace BHive
