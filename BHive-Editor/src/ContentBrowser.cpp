
#include "ContentBrowser.h"
#include "core/FileDialog.h"
#include "gfx/textures/Texture2D.h"

#define DRAG_DROP_SOURCE_TYPE "CONTENT_BROWSER_ITEM"

namespace BHive
{
	struct FileEntry
	{
		int mID;
		std::filesystem::directory_entry mEntry;
	};

	enum EContentBrowserAction : uint8_t
	{
		Action_None,
		Action_Delete,
		Action_Deselect,
		Action_Move
	};

	using SelectedItems = std::unordered_map<int, FileEntry>;
	static SelectedItems sSelectedItems;
	static int sActiveItem = -1;
	static EContentBrowserAction sCurrentAction = Action_None;
	static ImVec2 mStartDragPos{};

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
				open_settings = true;
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
		mIsMouseDragging = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);

		if (mIsMouseDragging)
		{
			if (!mDragStarting)
			{
				mStartDragPos = ImGui::GetIO().MousePos;
				mDragStarting = true;
			}
		}

		if (mDragStarting && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{

			mDragStarting = false;
			mStartDragPos = {0, 0};
		}
		ShowFileSystem();

		if (clicked)
		{
			sCurrentAction = Action_Deselect;
		}
		if (focused)
		{
			if (ImGui::IsKeyPressed(ImGuiKey_Delete))
			{
				sCurrentAction = Action_Delete;
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				sCurrentAction = Action_Deselect;
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

		ImGui::EndChild();

		if (open_settings)
		{
			if (ImGui::Begin("ContentBrowserSettings", &open_settings))
			{
				ImGui::SliderFloat("Padding", &mPadding, 1.f, 16.0f, "%.2f");
				ImGui::SliderFloat("ThumbnailSize", &mThumbnailSize, 32.0f, 128.0f, "%.2f");
				ImGui::End();
			}
		}

		// actions
		while (sCurrentAction != Action_None)
		{
			if (sCurrentAction == Action_Delete)
			{
				for (auto &[id, item] : sSelectedItems)
				{
					DeleteFolder(item.mEntry);
				}

				sCurrentAction = Action_Deselect;
			}

			if (sCurrentAction == Action_Deselect)
			{
				sActiveItem = -1;
				sSelectedItems.clear();
				sCurrentAction = Action_None;
			}
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
		auto drawlist = ImGui::GetWindowDrawList();

		ImGui::PushID(id.c_str());
		bool opened = ImGui::TreeNodeEx("##TreeNode", ImGuiTreeNodeFlags_SpanTextWidth);

		ImVec2 size = {200.f, GImGui->FontSize};

		ImGui::SameLine();
		if (ImGui::InvisibleButton("##icon", size, 0))
		{
			SetCurrentDirectory(mCurrentDirectory / directory);
		}

		auto rect = ImGui::GetItemRect();
		bool hovered = ImGui::IsItemHovered();
		bool active = ImGui::IsItemActive();
		auto icon = OnGetIcon(true, "");

		if (icon)
		{
			auto icon_color = hovered || active ? mStyle.mColors.mFolderHovered : mStyle.mColors.mFolder;
			auto frame_color = hovered ? ImGui::GetColorU32(ImGuiCol_FrameBgHovered) : IM_COL32(0, 0, 0, 0);

			drawlist->AddRectFilled(rect.Min, rect.Max, frame_color);
			drawlist->AddImage(
				(ImTextureID)(uint64_t)(uint32_t)*icon, rect.Min, rect.Min + ImVec2{size.y, size.y}, {0, 1}, {1, 0},
				icon_color);
			drawlist->AddText({rect.Min.x + size.y + GImGui->Style.FramePadding.x, rect.Min.y}, IM_COL32_WHITE, id.c_str());
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
		auto drawlist = ImGui::GetWindowDrawList();
		ImRect drag_rect{};

		if (mIsMouseDragging)
		{
			auto min = ImMin(mStartDragPos, ImGui::GetMousePos());
			auto max = ImMax(mStartDragPos, ImGui::GetMousePos());
			drag_rect = {min, max};
		}

		float cellsize = mThumbnailSize + mPadding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellsize);
		if (columnCount < 1)
			columnCount = 1;

		if (ImGui::BeginTable("##columns", columnCount, ImGuiTableFlags_PadOuterX))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			if (!mCurrentDirectory.empty())
			{
				auto directory_iter = std::filesystem::directory_iterator(mCurrentDirectory);

				int index = 0;
				for (auto &entry : directory_iter)
				{
					FileEntry ms_item = {index, entry};

					auto path = entry.path();
					auto ext = path.extension();
					bool is_directory = entry.is_directory();
					auto name = path.stem().string();

					auto relative_path = std::filesystem::relative(path, mBaseDirectory);
					bool is_valid_handle = IsAssetValid(relative_path);

					auto id = ImGui::GetID(path.string().c_str());

					ImGui::PushID(id);

					bool is_selected = sSelectedItems.contains(index);

					bool clicked = ImGui::Selectable("", is_selected, ImGuiSelectableFlags_AllowDoubleClick, mThumbnailSize);
					bool is_hovered = ImGui::IsItemHovered();
					auto rect = ImGui::GetItemRect();

					if (clicked)
					{
						if (!ImGui::IsKeyDown(ImGuiKey_ModCtrl))
						{
							sSelectedItems.clear();
						}

						sSelectedItems.emplace(index, ms_item);

						sActiveItem = index;
					}

					auto icon = OnGetIcon(is_directory, relative_path);
					if (icon)
					{
						auto color = is_directory ? mStyle.mColors.mFolder : IM_COL32_WHITE;
						drawlist->AddImage(
							(ImTextureID)(uint64_t)(uint32_t)*icon, rect.Min, rect.Max, {0, 1}, {1, 0}, color);
					}

					if (is_selected)
					{
						drawlist->AddRect(rect.Min, rect.Max, mStyle.mColors.mSelection, 0.f, 0, 2.0f);
					}

					if (is_valid_handle && !is_directory)
					{
						auto checkmark_size = 20.0f;
						ImGui::RenderCheckMark(
							drawlist, {rect.Max.x - checkmark_size, rect.Max.y - checkmark_size}, mStyle.mColors.mCheckMark,
							checkmark_size);
					}

					if (mIsMouseDragging)
					{

						if (drag_rect.Overlaps(rect))
						{
							if (!is_selected)
								sSelectedItems.emplace(index, ms_item);
						}
						else
						{
							if (is_selected)
							{
								sSelectedItems.erase(index);
							}
						}
					}

					if (is_directory)
					{
						if (ImGui::BeginDragDropTarget())
						{
							auto payload = ImGui::AcceptDragDropPayload("ASSET", ImGuiDragDropFlags_SourceAllowNullID);
							if (payload)
							{
								if (sSelectedItems.size())
								{
									for (auto &item : sSelectedItems)
									{
										OnRenameAsset(item.second.mEntry.path(), entry.path(), false);
									}

									sActiveItem = -1;
									sSelectedItems.clear();
								}
							}

							ImGui::EndDragDropTarget();
						}
					}

					if (ImGui::BeginDragDropSource())
					{
						UUID data;

						if (GetDragDropData(data, relative_path))
						{
							ImGui::SetDragDropPayload("ASSET", &data, sizeof(UUID));
						}

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
							sSelectedItems.emplace(index, ms_item);
							sCurrentAction == Action_Delete;
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
						auto new_path = relative_path.parent_path() / (new_name + relative_path.extension().string());
						OnRenameAsset(relative_path, new_path, is_directory);
					}

					ImGui::PopID();

					ImGui::TableNextColumn();

					index++;
				}
			}

			ImGui::EndTable();

			if (mIsMouseDragging)
			{
				drawlist->AddRect(drag_rect.Min, drag_rect.Max, IM_COL32(180, 200, 255, 255));
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

		FileDialogs::MoveToRecycleBin(item.path().string());
	}

	void ContentBrowserPanel::SetCurrentDirectory(const std::filesystem::path &path)
	{
		mCurrentDirectory = path;
		sActiveItem = -1;
		sSelectedItems.clear();
	}
} // namespace BHive