#include "ContentBrowserWindow.h"
#include "core/FileDialog.h"
#include "gfx/textures/Texture2D.h"
#include "gui/PayloadHelpers.h"

#define CREATE_ASSET_MENU_NAME "CREATE_ASSET_MENU"
#define CONTENT_BROWSER_SETTINGS_NAME "CONTENT_BROWSER_SETTINGS"

namespace BHive
{

	ImContentBrowserWindow::ImContentBrowserWindow()
	{
		mItems.reserve(200);
	}

	ImContentBrowserWindow::ImContentBrowserWindow(const std::filesystem::path &directory)
		: ImWindowBase(ImGuiWindowFlags_MenuBar),
		  mBaseDirectory(directory),
		  mCurrentDirectory(directory)
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

		SetBaseDirectory(directory);
	}

	void ImContentBrowserWindow::OnUpdateContent()
	{

		static bool open_settings = false;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::Button("Settings"))
			{
				ImGui::OpenPopup(CONTENT_BROWSER_SETTINGS_NAME);
			}

			if (ImGui::BeginPopup(CONTENT_BROWSER_SETTINGS_NAME))
			{
				ImGui::SliderFloat("Padding", &mPadding, 1.f, 16.0f, "%.2f");
				ImGui::SliderFloat("ThumbnailSize", &mThumbnailSize, 32.0f, 128.0f, "%.2f");
				ImGui::EndPopup();
			}

			ImGui::PushStyleColor(ImGuiCol_Button, 0xff4cb024);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0xff67da38);

			if (ImGui::Button("+ Add"))
			{
				ImGui::OpenPopup(CREATE_ASSET_MENU_NAME);
			}

			ImGui::PopStyleColor(2);

			if (ImGui::BeginPopup(CREATE_ASSET_MENU_NAME))
			{
				OnCreateAssetMenu();

				ImGui::EndPopup();
			}

			if (ImGui::Button("Import"))
			{
				auto path_str = FileDialogs::OpenFile("All (*.*)\0*.*\0 Mesh (*.glb;*.gltf)\0*.glb;*.gltf\0");
				if (!path_str.empty())
					OnImportAsset(mCurrentDirectory, path_str);
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

		ImGui::BeginChild("##TreeView", {200, 0}, ImGuiChildFlags_ResizeX, ImGuiWindowFlags_AlwaysUseWindowPadding);

		ShowFileSystemTree(mTree);

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("##AssetView", {0, 0}, 0, ImGuiWindowFlags_AlwaysUseWindowPadding);

		bool focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
		bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
		bool clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

		ShowFileSystem();

		ImGui::EndChild();

		if (focused)
		{
			if (ImGui::IsKeyPressed(ImGuiKey_Delete))
			{
				mContentBrowerAction = [=]()
				{
					void *iterator = nullptr;
					ImGuiID id = 0;
					while (mSelection.GetNextSelectedItem(&iterator, &id))
					{
						auto it = std::find_if(mItems.begin(), mItems.end(), [id](const FileEntry &obj) { return obj.ID == id; });
						if (it != mItems.end())
						{
							DeleteFolder(it->Entry);
						}
					}
				};
			}
		}

		if (mContentBrowerAction)
		{
			mContentBrowerAction();
			mContentBrowerAction = nullptr;
		}
	}

	void ImContentBrowserWindow::SetBaseDirectory(const std::filesystem::path &directory)
	{
		mBaseDirectory = directory;
		SetCurrentDirectory(mBaseDirectory);

		if (std::filesystem::is_directory(directory))
		{
			mTree = ImTreeFolder(std::filesystem::directory_entry(directory));
			ConstructTree(mTree);
		}
	}

	void ImContentBrowserWindow::ShowFileSystemTree(ImTreeFolder &folder)
	{
		auto directory = folder.GetEntry();
		auto id = folder.GetID();

		ImGui::PushID(directory.path().string().c_str());

		bool opened = ImGui::TreeNodeEx("##TreeNode", ImGuiTreeNodeFlags_SpanTextWidth);

		ImGui::SameLine();

		auto icon = OnGetIcon(directory);
		auto pressed = folder.Draw((ImTextureID)(uint64_t)(uint32_t)*icon, 200.0f, mStyle.mColors.mFolder, mStyle.mColors.mFolderHovered);
		if (pressed)
		{
			SetCurrentDirectory(mCurrentDirectory / directory);
		}

		if (directory.is_directory())
		{
			if (ImGui::BeginDragDropTargetCustom(ImGui::GetItemRect(), id))
			{
				ApplyDragDropTarget(directory);

				ImGui::EndDragDropTarget();
			}
		}

		if (opened)
		{
			for (auto &child : folder.GetChildren())
			{
				ShowFileSystemTree(child);
			}

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	void ImContentBrowserWindow::ShowFileSystem()
	{
		if (mCurrentDirectory.empty())
			return;

		mItems.clear();

		auto directory_iter = std::filesystem::directory_iterator(mCurrentDirectory);

		// set items from directory
		for (const auto &entry : directory_iter)
		{
			auto id = ImGui::GetID(entry.path().string().c_str());
			mItems.emplace_back(id, entry);
		}

		float cellsize = mThumbnailSize + mPadding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellsize);
		if (columnCount < 1)
			columnCount = 1;

		// Begin Multiselection
		auto selection_flags = ImGuiMultiSelectFlags_ClearOnClickVoid | ImGuiMultiSelectFlags_ClearOnEscape | ImGuiMultiSelectFlags_BoxSelect2d;
		ImGuiMultiSelectIO *selection_io = ImGui::BeginMultiSelect(selection_flags, mSelection.Size, mItems.size());
		mSelection.ApplyRequests(selection_io);
		mSelection.UserData = (void *)&mItems;
		mSelection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage *self, int idx) { return (*((EntryItems *)self->UserData))[idx].ID; };

		if (ImGui::BeginTable("##columns", columnCount, ImGuiTableFlags_PadOuterX))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			// draw items
			if (!mCurrentDirectory.empty())
			{
				auto directory_iter = std::filesystem::directory_iterator(mCurrentDirectory);

				auto drag_drop_target_callback = [=](const auto &entry) { ApplyDragDropTarget(entry); };
				auto renamed_callback = [=](const auto &_old, const auto &_new) { OnRenameAsset(_old, _new); };
				auto drag_drop_source_callback = [=](const auto &entry)
				{
					if (ImGui::GetDragDropPayload() == nullptr)
					{
						std::vector<std::filesystem::directory_entry> entries;

						void *iterator = nullptr;
						ImGuiID id = 0;
						while (mSelection.GetNextSelectedItem(&iterator, &id))
						{
							auto it = std::find_if(mItems.begin(), mItems.end(), [id](const FileEntry &obj) { return obj.ID == id; });
							if (it != mItems.end())
								entries.emplace_back(it->Entry);
						}

						size_t data_size = 0;
						auto buffer = PayloadHelpers::get_entries_buffer(entries, data_size);
						if (!buffer.empty())
						{
							ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", buffer.c_str(), data_size);
						}
					}
				};

				auto double_clicked_callback = [=](const auto &e, bool directory)
				{
					if (directory)
					{
						SetCurrentDirectory(mCurrentDirectory / e.path().filename());
					}
					else
					{

						OnAssetDoubleClicked(e.path());
					}
				};

				for (size_t i = 0; i < mItems.size(); i++)
				{
					FileEntry file_entry = mItems[i];

					auto id = file_entry.ID;
					bool is_valid_handle = IsAssetValid(file_entry.Entry);
					bool is_selected = mSelection.Contains(id);
					auto icon = OnGetIcon(file_entry.Entry);

					auto context_menu_callback = [=](const auto &e, bool directory)
					{
						if (ImGui::MenuItem("Delete"))
						{
							mContentBrowerAction = [=]() { DeleteFolder(e); };
						}

						if (!directory)
						{
							if (is_valid_handle)
							{
								OnAssetContextMenu(e);
							}
							else
							{
								if (e.path().extension() == ".asset")
								{
									if (ImGui::MenuItem("Import"))
									{
										OnReimportAsset(e);
									}
								}
							}
						}
					};

					ImGui::SetNextItemSelectionUserData(i);

					ImDirectoryEntry directory_entry(file_entry.Entry);
					directory_entry.SetDoubleClickedCallback(double_clicked_callback);
					directory_entry.SetDragDropTargetCallback(drag_drop_target_callback);
					directory_entry.SetDragDropSourceCallback(drag_drop_source_callback);
					directory_entry.SetRenamedCallback(renamed_callback);
					directory_entry.SetContextMenuCallback(context_menu_callback);
					directory_entry.Draw((ImTextureID)(uint64_t)(uint32_t)*icon, mThumbnailSize, is_selected, is_valid_handle, mStyle.mColors.mFolder);

					ImGui::TableNextColumn();
				}
			}

			ImGui::EndTable();
		}

		selection_io = ImGui::EndMultiSelect();
		mSelection.ApplyRequests(selection_io);

		if (ImGui::BeginPopupContextWindow("Context", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			OnCreateAssetMenu();

			ImGui::EndPopup();
		}
	}

	void ImContentBrowserWindow::OnDeleteFolder(const std::filesystem::directory_entry &entry)
	{
		if (!entry.is_directory())
		{
			OnDeleteAsset(entry);
		}

		if (entry.is_directory())
		{
			for (auto &child_entry : std::filesystem::directory_iterator(entry))
			{
				OnDeleteFolder(child_entry);
			}
		}
	}

	void ImContentBrowserWindow::DeleteFolder(const std::filesystem::directory_entry &item)
	{
		OnDeleteFolder(item);

		FileDialogs::MoveToRecycleBin(item.path().string());
	}

	void ImContentBrowserWindow::SetCurrentDirectory(const std::filesystem::path &path)
	{
		mCurrentDirectory = path;
		mSelection.Clear();
	}

	void ImContentBrowserWindow::ApplyDragDropTarget(const std::filesystem::directory_entry &entry)
	{
		mContentBrowerAction = [=]()
		{
			if (auto payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM", ImGuiDragDropFlags_SourceAllowNullID))
			{
				auto data = (const char *)payload->Data;
				auto entries = PayloadHelpers::get_entries_from_buffer(data, payload->DataSize);

				for (const auto &other_entry : entries)
				{
					auto new_path = entry.path() / other_entry.path().filename();
					OnRenameAsset(other_entry, new_path);
				}
			}
		};
	}

	void ImContentBrowserWindow::ConstructTree(ImTreeFolder &folder)
	{
		for (auto &entry : std::filesystem::directory_iterator(folder.GetEntry()))
		{
			if (!entry.is_directory())
				continue;

			auto child = ImTreeFolder(entry);
			ConstructTree(child);
			folder.AddChild(child);
		}
	}

	void ImContentBrowserWindow::OnCreateAssetMenu()
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
	}
} // namespace BHive