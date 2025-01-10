
#include "ContentBrowser.h"

#include "importers/TextureImporter.h"
#include "gfx/Texture.h"
#include "core/FileDialog.h"
#include "gui/ImGuiExtended.h"

#define DRAG_DROP_SOURCE_TYPE "CONTENT_BROWSER_ITEM"

namespace BHive
{
	static std::vector<const char *> sHiddenExtensions = {".registry"};

	struct InpsectedEntryItem
	{
		std::filesystem::directory_entry mEntry;
	};

	ContentBrowserPanel::ContentBrowserPanel(const std::filesystem::path &directory)
		: mBaseDirectory(directory), mCurrentDirectory(directory)
	{
		if (!std::filesystem::exists(directory))
		{
			std::error_code error;
			bool created = std::filesystem::create_directory(directory, error);
			if (created)
			{
				LOG_TRACE("Create directory {}", directory.string());
			}
			else
				LOG_ERROR("Failed to create directory - {}", error.message());
		}
	}

	void ContentBrowserPanel::OnGuiRender()
	{
		static bool open_settings = false;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::Button("Settings"))
			{
				open_settings = true;
			}

			if (ImGui::Button("Import"))
			{
				auto path_str = FileDialogs::OpenFile("All (*.*)\0*.*\0 Mesh (*.glb;*.gltf)\0*.glb;*.gltf\0");
				if (!path_str.empty()) OnImportAsset(mCurrentDirectory ,path_str);
			}

			if (mCurrentDirectory != mBaseDirectory)
			{
				if (ImGui::Button("<-"))
				{
					mCurrentDirectory = mCurrentDirectory.parent_path();
				}
			}
			ImGui::EndMenuBar();
		}

		ImGui::BeginTable("##Content", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable);
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		ShowFileSystemTree(std::filesystem::directory_entry(mBaseDirectory));

		ImGui::TableNextColumn();

		static float padding = 16.0f;
		static float thumbnailsize = 128.0f;
		float cellsize = thumbnailsize + padding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellsize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::BeginTable("##columns", columnCount);
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		ShowFileSystem(thumbnailsize);

		ImGui::EndTable();

		ImGui::EndTable();

		if (open_settings)
		{
			if (ImGui::Begin("ContentBrowserSettings", &open_settings))
			{
				ImGui::SliderFloat("Padding", &padding, 1.f, 16.0f, "%.2f");
				ImGui::SliderFloat("ThumbnailSize", &thumbnailsize, 32.0f, 128.0f, "%.2f");
				ImGui::End();
			}
		}

		if (ImGui::BeginPopupContextWindow("Context", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::Selectable("Create Folder"))
			{
				std::error_code error;
				std::filesystem::create_directory(mCurrentDirectory / "NewFolder", error);
				if (error)
				{
					LOG_ERROR("{}", error.message());
				}
			}

			OnWindowContextMenu();

			ImGui::EndPopup();
		}
	}

	void ContentBrowserPanel::SetBaseDirectory(const std::filesystem::path &directory)
	{
		mBaseDirectory = directory;
		mCurrentDirectory = directory;
	}

	void ContentBrowserPanel::ShowFileSystemTree(const std::filesystem::directory_entry &directory)
	{
		auto id = directory.path().stem().string();
		auto icon = OnGetIcon(true, "");
		if (ImGui::DrawIcon("##" + id, icon.get(), GImGui->FontSize, 0))
		{
			mCurrentDirectory = directory;
		}

		ImGui::SameLine();

		if (ImGui::TreeNodeEx(id.c_str()))
		{
			for (auto &entry : std::filesystem::directory_iterator(directory))
			{
				if (entry.is_directory())
					ShowFileSystemTree(entry);
			}

			ImGui::TreePop();
		}
	}

	void ContentBrowserPanel::ShowFileSystem(float thumbnailsize)
	{
		static bool delete_entry = false;
		static InpsectedEntryItem inspected_item;

		if (!mCurrentDirectory.empty())
		{
			auto directory_iter = std::filesystem::directory_iterator(mCurrentDirectory);
			for (auto &entry : directory_iter)
			{
				auto path = entry.path();
				auto ext = path.extension();

				if (std::find(sHiddenExtensions.begin(), sHiddenExtensions.end(), ext) != sHiddenExtensions.end())
					continue;

				bool is_directory = entry.is_directory();
				auto name = path.filename().string();

				auto relative_path = std::filesystem::relative(path, mBaseDirectory);
				bool is_valid_handle = IsAssetValid(relative_path);

				auto icon = OnGetIcon(is_directory, relative_path);
				ImGui::PushID(path.string().c_str());

				ImGui::DrawIcon(name.c_str(), icon.get(), thumbnailsize, 0);

				if (ImGui::BeginDragDropSource())
				{
					AssetHandle data;

					if (GetDragDropData(data, relative_path))
					{
						ImGui::SetDragDropPayload("ASSET", &data, sizeof(AssetHandle));
					}

					ImGui::EndDragDropSource();
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (is_directory)
					{
						mCurrentDirectory /= path.filename();
					}
					else
					{
						OnAssetDoubleClicked(relative_path);
					}
				}

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete"))
					{
						inspected_item = {entry};
						delete_entry = true;
					}

					if (!is_directory)
					{
						if (is_valid_handle)
						{
							OnAssetContextMenu(relative_path);
						}
					}
					ImGui::EndPopup();
				}

				static std::string file_name;
				if (ImGui::DrawEditableText(path.stem().string().c_str(), name, file_name))
				{
					auto new_path = relative_path.parent_path() /
									(file_name + relative_path.extension().string());
					OnRenameAsset(relative_path, new_path, is_directory);
				}

				ImGui::PopID();

				ImGui::TableNextColumn();
			}

			if (delete_entry)
			{
				ImGui::OpenPopup("Delete?");
			}

			ImGui::SetNextWindowSize({ 100, 100 });
			if (ImGui::BeginPopupModal("Delete?", &delete_entry, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize))
			{

				if (ImGui::Button("Yes"))
				{
					DeleteFolder(inspected_item.mEntry);
					delete_entry = false;
				}

				ImGui::SameLine();

				if (ImGui::Button("No"))
				{
					delete_entry = false;
				}

				ImGui::EndPopup();
			}
		}
	}
	void ContentBrowserPanel::DeleteFolder(const std::filesystem::directory_entry &item)
	{
		if (!item.is_directory())
		{
			auto relative_path = std::filesystem::relative(item.path(), mBaseDirectory);
			OnDeleteAsset(relative_path);
			return;
		}

		for (auto &entry : std::filesystem::directory_iterator(item))
		{
			DeleteFolder(entry);
		}

		std::error_code error;
		if (std::filesystem::remove(item, error))
		{
			LOG_TRACE("Deleted folder {}", item.path().string());
		}
		else
		{
			LOG_ERROR("Failed to delete folder {}", error.message());
		}
	}
}