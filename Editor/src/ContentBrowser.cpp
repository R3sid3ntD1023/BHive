
#include "ContentBrowser.h"
#include "core/FileDialog.h"
#include "gfx/Texture.h"
#include "gui/ImGuiExtended.h"

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
		Action_Move
	};

	using SelectedItems = std::unordered_map<int, FileEntry>;
	static SelectedItems sSelectedItems;
	static int sActiveItem = -1;
	static EContentBrowserAction sCurrentAction = Action_None;

	ContentBrowserPanel::ContentBrowserPanel(const std::filesystem::path &directory)
		: mBaseDirectory(directory),
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
				auto path_str =
					FileDialogs::OpenFile("All (*.*)\0*.*\0 Mesh (*.glb;*.gltf)\0*.glb;*.gltf\0");
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

		ImGui::BeginTable(
			"##Content", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable);
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		ShowFileSystemTree(std::filesystem::directory_entry(mBaseDirectory));

		ImGui::TableNextColumn();

		ShowFileSystem();

		ImGui::EndTable();

		if (open_settings)
		{
			if (ImGui::Begin("ContentBrowserSettings", &open_settings))
			{
				ImGui::SliderFloat("Padding", &mPadding, 1.f, 16.0f, "%.2f");
				ImGui::SliderFloat("ThumbnailSize", &mThumbnailSize, 32.0f, 128.0f, "%.2f");
				ImGui::End();
			}
		}

		if (ImGui::IsWindowFocused())
		{
			if (ImGui::IsKeyPressed(ImGuiKey_Delete))
			{
				sCurrentAction = Action_Delete;
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				sActiveItem = -1;
				sSelectedItems.clear();
			}
		}

		if (sCurrentAction != Action_None)
		{
			if (sCurrentAction == Action_Delete)
			{
				for (auto &[id, item] : sSelectedItems)
				{
					DeleteFolder(item.mEntry);
				}
			}

			sCurrentAction = Action_None;
			sActiveItem = -1;
			sSelectedItems.clear();
		}

		if (ImGui::BeginPopupContextWindow(
				"Context", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
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

	void ContentBrowserPanel::ShowFileSystem()
	{
		auto drawlist = ImGui::GetWindowDrawList();

		float cellsize = mThumbnailSize + mPadding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellsize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::BeginTable("##columns", columnCount);
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

				auto icon = OnGetIcon(is_directory, relative_path);

				auto id = ImGui::GetID(path.string().c_str());

				ImGui::PushID(id);

				if (ImGui::DrawIcon(name.c_str(), icon.get(), mThumbnailSize, 0))
				{
					if (!ImGui::IsKeyDown(ImGuiKey_ModCtrl))
					{
						sSelectedItems.clear();
					}

					sSelectedItems.emplace(index, ms_item);

					sActiveItem = index;
				}

				bool is_selected = sSelectedItems.contains(index);
				if (is_selected)
				{
					auto rect = ImGui::GetItemRect();
					drawlist->AddRect(rect.Min, rect.Max, 0xFF00FFFF, 0.f, 0, 2.0f);
				}

				if (is_directory)
				{
					if (ImGui::BeginDragDropTarget())
					{
						auto payload = ImGui::AcceptDragDropPayload(
							"ASSET", ImGuiDragDropFlags_SourceAllowNullID);
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
					AssetHandle data;

					if (GetDragDropData(data, relative_path))
					{
						ImGui::SetDragDropPayload("ASSET", &path, sizeof(AssetHandle));
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

				static std::string file_name;
				if (ImGui::DrawEditableText(name.c_str(), name, file_name))
				{
					auto new_path = path.parent_path() / (file_name + path.extension().string());
					OnRenameAsset(path, new_path, is_directory);
				}

				if (!is_directory)
				{
					ImGui::Checkbox("##AssetValid", &is_valid_handle);
				}
				ImGui::PopID();

				ImGui::TableNextColumn();

				index++;
			}

			ImGui::EndTable();
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