
#include "ContentBrowser.h"
#include "core/FileDialog.h"
#include "gfx/textures/Texture2D.h"

#define DRAG_DROP_SOURCE_TYPE "CONTENT_BROWSER_ITEM"
#define CREATE_ASSET_MENU_NAME "CREATE_ASSET_MENU"
#define CONTENT_BROWSER_SETTINGS_NAME "CONTENT_BROWSER_SETTINGS"

namespace BHive
{

	struct FilesPayload
	{
		static auto get_buffer(const std::vector<std::filesystem::directory_entry> &entries, size_t &size)
		{
			std::string buffer;
			for (const auto &str : entries)
			{
				buffer += str.path().string() + '\0';
				size += buffer.size() + 1;
			}
			return buffer;
		}

		static auto get_entries(const char *buffer, size_t size)
		{
			std::vector<std::filesystem::directory_entry> entries;
			const char *current = buffer;
			const char *end = buffer + size;

			while (current < end)
			{
				std::string str(current);
				if (!str.empty())
					entries.emplace_back(str);
				current += str.size() + 1;
			}

			return entries;
		}
	};

	ContentBrowserPanel::ContentBrowserPanel()
	{
		mItems.reserve(200);
	}

	ContentBrowserPanel::ContentBrowserPanel(const std::filesystem::path &directory)
		: WindowBase(ImGuiWindowFlags_MenuBar),
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

		SetCurrentDirectory(directory);
	}

	void ContentBrowserPanel::OnGuiRender()
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

		ImGui::BeginChild("TreeView", {200, 0}, ImGuiChildFlags_ResizeX, ImGuiWindowFlags_AlwaysUseWindowPadding);
		ShowFileSystemTree(std::filesystem::directory_entry(mBaseDirectory));
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("AssetView", {0, 0}, 0, ImGuiWindowFlags_AlwaysUseWindowPadding);

		bool focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
		bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
		bool clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

		ShowFileSystem();

		if (ImGui::BeginPopupContextWindow("Context", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			OnCreateAssetMenu();

			ImGui::EndPopup();
		}

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

	void ContentBrowserPanel::SetBaseDirectory(const std::filesystem::path &directory)
	{
		mBaseDirectory = directory;
		SetCurrentDirectory(mBaseDirectory);
	}

	void ContentBrowserPanel::ShowFileSystemTree(const std::filesystem::directory_entry &directory)
	{
		auto name = directory.path().stem().string();
		auto id = ImGui::GetID(directory.path().filename().string().c_str());
		auto drawlist = ImGui::GetWindowDrawList();

		ImGui::PushID(id);
		bool opened = ImGui::TreeNodeEx("##TreeNode", ImGuiTreeNodeFlags_SpanTextWidth);

		ImVec2 size = {200.f, ImGui::GetFontSize()};

		ImGui::SameLine();
		if (ImGui::InvisibleButton("##icon", size, 0))
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

		auto rect = ImGui::GetItemRect();
		bool hovered = ImGui::IsItemHovered();
		bool active = ImGui::IsItemActive();
		auto icon = OnGetIcon(directory);

		if (icon)
		{
			auto icon_color = hovered || active ? mStyle.mColors.mFolderHovered : mStyle.mColors.mFolder;
			auto frame_color = hovered ? ImGui::GetColorU32(ImGuiCol_FrameBgHovered) : IM_COL32(0, 0, 0, 0);

			drawlist->AddRectFilled(rect.Min, rect.Max, frame_color);
			drawlist->AddImage((ImTextureID)(uint64_t)(uint32_t)*icon, rect.Min, rect.Min + ImVec2{size.y, size.y}, {0, 1}, {1, 0}, icon_color);
			drawlist->AddText({rect.Min.x + size.y + GImGui->Style.FramePadding.x, rect.Min.y}, IM_COL32_WHITE, name.c_str());
		}

		if (opened)
		{
			for (auto &entry : std::filesystem::directory_iterator(directory))
			{
				if (entry.is_directory())
					ShowFileSystemTree(entry);
			}

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	void ContentBrowserPanel::ShowFileSystem()
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
		auto selection_flags = ImGuiMultiSelectFlags_ClearOnClickVoid | ImGuiMultiSelectFlags_ClearOnEscape | ImGuiMultiSelectFlags_BoxSelect2d | ImGuiMultiSelectFlags_ScopeWindow;
		ImGuiMultiSelectIO *selection_io = ImGui::BeginMultiSelect(selection_flags, mSelection.Size, mItems.size());
		mSelection.ApplyRequests(selection_io);
		mSelection.UserData = (void *)&mItems;
		mSelection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage *self, int idx) { return (*((EntryItems *)self->UserData))[idx].ID; };

		if (ImGui::BeginTable("##columns", columnCount, ImGuiTableFlags_PadOuterX))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			auto drawlist = ImGui::GetWindowDrawList();

			// draw items
			if (!mCurrentDirectory.empty())
			{
				auto directory_iter = std::filesystem::directory_iterator(mCurrentDirectory);

				for (size_t i = 0; i < mItems.size(); i++)
				{
					FileEntry file_entry = mItems[i];

					auto path = file_entry.Entry.path();
					auto ext = path.extension();
					bool is_directory = file_entry.Entry.is_directory();
					auto name = path.stem().string();
					auto id = file_entry.ID;
					auto relative_path = std::filesystem::relative(path, mBaseDirectory);
					bool is_valid_handle = IsAssetValid(relative_path);
					bool is_selected = mSelection.Contains(id);

					auto icon = OnGetIcon(file_entry.Entry);

					ImGui::PushID(id);

					ImGui::SetNextItemSelectionUserData(i);
					bool clicked = ImGui::Selectable("", is_selected, ImGuiSelectableFlags_AllowDoubleClick, mThumbnailSize);
					bool is_hovered = ImGui::IsItemHovered();
					auto rect = ImGui::GetItemRect();

					if (icon)
					{
						auto color = is_directory ? mStyle.mColors.mFolder : IM_COL32_WHITE;
						drawlist->AddImage((ImTextureID)(uint64_t)(uint32_t)*icon, rect.Min, rect.Max, {0, 1}, {1, 0}, color);
					}

					/*	if (is_selected)
						{
							drawlist->AddRect(rect.Min, rect.Max, mStyle.mColors.mSelection, 0.f, 0);
						}*/

					if (is_valid_handle && !is_directory)
					{
						auto checkmark_size = 20.0f;
						ImGui::RenderCheckMark(drawlist, {rect.Max.x - checkmark_size, rect.Max.y - checkmark_size}, mStyle.mColors.mCheckMark, checkmark_size);
					}

					if (is_directory)
					{
						if (ImGui::BeginDragDropTarget())
						{
							ApplyDragDropTarget(file_entry.Entry);

							ImGui::EndDragDropTarget();
						}
					}

					if (ImGui::BeginDragDropSource())
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
							auto buffer = FilesPayload::get_buffer(entries, data_size);
							if (!buffer.empty())
							{
								ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", buffer.c_str(), data_size);
							}
						}

						if (icon)
							ImGui::Image(icon.get(), mThumbnailSize);
						ImGui::TextUnformatted(name.c_str());

						ImGui::EndDragDropSource();
					}

					if (is_hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
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
							mContentBrowerAction = [=]() { DeleteFolder(file_entry.Entry); };
						}

						if (!is_directory)
						{
							if (is_valid_handle)
							{

								OnAssetContextMenu(relative_path);
							}
							else
							{
								if (path.extension() == ".asset")
								{
									if (ImGui::MenuItem("Import"))
									{
										OnReimportAsset(relative_path);
									}
								}
							}
						}
						ImGui::EndPopup();
					}

					std::string new_name;
					bool edited_name = ImGui::DrawEditableText(name, new_name);

					if (edited_name)
					{
						auto new_path = path.parent_path() / (new_name + relative_path.extension().string());
						OnRenameAsset(path, new_path);
					}

					ImGui::PopID();

					ImGui::TableNextColumn();
				}
			}

			ImGui::EndTable();
		}

		selection_io = ImGui::EndMultiSelect();
		mSelection.ApplyRequests(selection_io);
	}

	void ContentBrowserPanel::OnDeleteFolder(const std::filesystem::directory_entry &entry)
	{
		if (!entry.is_directory())
		{
			auto relative_path = std::filesystem::relative(entry.path(), mBaseDirectory);
			OnDeleteAsset(relative_path);
		}

		if (entry.is_directory())
		{
			for (auto &child_entry : std::filesystem::directory_iterator(entry))
			{
				OnDeleteFolder(child_entry);
			}
		}
	}

	void ContentBrowserPanel::DeleteFolder(const std::filesystem::directory_entry &item)
	{
		OnDeleteFolder(item);

		FileDialogs::MoveToRecycleBin(item.path().string());
	}

	void ContentBrowserPanel::SetCurrentDirectory(const std::filesystem::path &path)
	{
		mCurrentDirectory = path;
		mSelection.Clear();
	}

	void ContentBrowserPanel::ApplyDragDropTarget(const std::filesystem::directory_entry &entry)
	{
		mContentBrowerAction = [=]()
		{
			if (auto payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM", ImGuiDragDropFlags_SourceAllowNullID))
			{
				auto data = (const char *)payload->Data;
				auto entries = FilesPayload::get_entries(data, payload->DataSize);

				for (const auto &other_entry : entries)
				{
					auto new_path = entry.path() / other_entry.path().filename();
					OnRenameAsset(other_entry, new_path);
				}
			}
		};
	}

	void ContentBrowserPanel::OnCreateAssetMenu()
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