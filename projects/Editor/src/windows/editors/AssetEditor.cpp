#include "AssetEditor.h"
#include "core/FileDialog.h"

namespace BHive
{
	AssetEditor::AssetEditor(int flags)
		: WindowBase(ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_HorizontalScrollbar | flags)
	{
	}

	void AssetEditor::OnUpdateContent()
	{
		if (ImGui::BeginMenuBar())
		{
			MenuBar();

			ImGui::EndMenuBar();
		}

		OnWindowRender();
	}

	void AssetEditor::MenuBar()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{
				OnSave(mCurrentSavePath);
			}

			if (ImGui::MenuItem("SaveAs"))
			{
				auto path = FileDialogs::SaveFile("Asset (*.asset)\0*.asset");
				if (!path.empty())
				{
					if (OnSave(path))
					{
						mCurrentSavePath = path;
					}
				}
			}

			ImGui::EndMenu();
		}

		OnMenuBar();
	}

} // namespace BHive
