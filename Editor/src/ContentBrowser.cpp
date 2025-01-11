
#include "ContentBrowser.h"

#include "importers/TextureImporter.h"
#include "gfx/Texture.h"
#include "core/FileDialog.h"
#include "gui/ImGuiExtended.h"

#define DRAG_DROP_SOURCE_TYPE "CONTENT_BROWSER_ITEM"

namespace BHive
{
	static std::vector<const char *> sHiddenExtensions = {".registry"};

	struct FileEntry
	{
		int mID;
		std::filesystem::directory_entry mEntry;
	};

	using SelectedItems = std::unordered_map<int, FileEntry>;
	static SelectedItems sSelectedItems;
	static int sActiveItem = -1;
	static std::vector<FileEntry> sItems;

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

		SetCurrentDirectory(directory);
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
					SetCurrentDirectory(mCurrentDirectory.parent_path());
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
		SetCurrentDirectory(mBaseDirectory);
	}

	void ContentBrowserPanel::ShowFileSystemTree(const std::filesystem::directory_entry &directory)
	{
		auto id = directory.path().stem().string();
		auto icon = OnGetIcon(true, "");
		if (ImGui::DrawIcon("##" + id, icon.get(), GImGui->FontSize, 0))
		{
			SetCurrentDirectory(directory);
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
		static bool delete_entries = false;

		if (!mCurrentDirectory.empty())
		{
			auto directory_iter = std::filesystem::directory_iterator(mCurrentDirectory);
			
			int index = 0;

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
				
				auto id = ImGui::GetID(path.string().c_str());
				
				ImGui::PushID(id);

				bool is_selected = sSelectedItems.contains(index);
				if (ImGui::DrawIcon(name.c_str(), icon.get(), thumbnailsize, 0, &is_selected))
				{
					FileEntry ms_item = {index, entry}; 

					if (!ImGui::IsKeyDown(ImGuiKey_ModCtrl))
					{
			
						sSelectedItems.clear();
					}

					if (ImGui::IsKeyDown(ImGuiKey_ModShift))
					{
						if (sActiveItem != -1)
						{
							auto start = sActiveItem;
							auto end = index;

							for (int i = start; i <= end; i++)
							{
								auto &item = sItems[i];
								sSelectedItems.emplace(item.mID, item);
							}
						}
					}
					else
					{
			
						sSelectedItems.emplace(index, ms_item);
					}

					sActiveItem = index;
				}



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
						SetCurrentDirectory(mCurrentDirectory / path.filename());
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
						delete_entries = true;
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

				 index++;
			}

			if (ImGui::IsWindowFocused())
			{
				if (ImGui::IsKeyPressed(ImGuiKey_Delete))
				{
					delete_entries = true;
				}
				else if (ImGui::IsKeyPressed(ImGuiKey_Escape))
				{
					sActiveItem = -1;
					sSelectedItems.clear();
				}
			}

			if (delete_entries)
			{
				for (auto &[id, item] : sSelectedItems)
				{
					DeleteFolder(item.mEntry);
				}
				delete_entries = false;

				sActiveItem = -1;
				sSelectedItems.clear();
			}
		}
	}

	void ContentBrowserPanel::OnDeleteFolder(const std::filesystem::directory_entry &entry) 
	{
		if (!entry.is_directory())
		{
			auto relative_path = std::filesystem::relative(entry.path(), mBaseDirectory);
			OnDeleteAsset(relative_path);
			return;
		}

		for (auto &child_entry : std::filesystem::directory_iterator(entry))
		{
			OnDeleteFolder(child_entry);
		}
	}

	void ContentBrowserPanel::DeleteFolder(const std::filesystem::directory_entry &item)
	{
		OnDeleteFolder(item);

		if (!FileDialogs::MoveToRecycleBin(item.path().string()))
		{
			LOG_ERROR("Failed to delete folder {}");
		}
	}
	void ContentBrowserPanel::SetCurrentDirectory(const std::filesystem::path &path)
	{
		mCurrentDirectory = path;
		sItems.clear();
		sActiveItem = -1;
		sSelectedItems.clear();

		auto directory_iter = std::filesystem::directory_iterator(mCurrentDirectory);

		int index = 0;
		for (auto& entry : directory_iter)
		{
			sItems.push_back(FileEntry{index, entry});
			index++;
		}
	}
}