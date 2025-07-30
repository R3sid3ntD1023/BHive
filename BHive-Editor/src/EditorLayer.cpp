#include "EditorLayer.h"
#include "asset/AssetFactory.h"
#include "contextmenus/ContextMenus.h"
#include "core/Application.h"
#include "core/FileDialog.h"
#include "core/layers/ImGuiLayer.h"
#include "core/subsystem/SubSystem.h"
#include "core/Window.h"
#include "gfx/Framebuffer.h"
#include "gfx/RenderCommand.h"
#include "GUI/Gui.h"
#include "gui/ImGuiExtended.h"
#include "ImGuizmo.h"
#include "inspectors/Inspect.h"
#include "LogPanel.h"
#include "renderers/Renderer.h"
#include "renderers/SceneRenderer.h"
#include "subsystems/SelectionSubSystem.h"
#include "subsystems/WindowSubSystem.h"
#include "world/GameObject.h"
#include "undoredo/UndoRedo.h"
#include "windows//HistoryWindow.h"
#include "undoredo/Commands.h"
#include <rttr/library.h>

namespace BHive
{
	std::unordered_map<uint8_t, float> sSnapValues = {
		{ImGuizmo::TRANSLATE, 10.f}, {ImGuizmo::ROTATE, 15.f}, {ImGuizmo::SCALE, .25f}};

	void EditorLayer::OnAttach()
	{
		if (!Project::GetActive())
		{
			auto project_path = FileDialogs::OpenFile("BHive-Project (*proj)\0 *.proj\0");
			if (project_path.empty())
			{
				Application::Get().Close();
				return;
			}

			Project::LoadProject(project_path);
		}

		auto project_name = Project::GetProjectName();
		mProjectLib = new rttr::library(project_name);
		if (!mProjectLib->is_loaded())
		{
			mProjectLib->load();
		}

		FWorldContentMenu::OnAssetOpenedEvent.bind(
			[&](const UUID &handle)
			{
				auto asset = AssetManager::GetAsset<World>(handle);

				if (asset)
				{
					SubSystemContext::Get().GetSubSystem<SelectionSubSystem>().Clear();
					mEditorWorld = asset;
					SetActiveWorld(mEditorWorld);

					auto &meta_data = mAssetManager->GetMetaData(handle);
					mCurrentWorldPath = Project::GetResourceDirectory() / meta_data.Path;
					auto &window = Application::Get().GetWindow();
					window.SetTitle(mCurrentWorldPath.stem().string());
				}
			});

		auto &window = Application::Get().GetWindow();
		auto &size = window.GetSize();

		mRenderer = CreateRef<SceneRenderer>();
		mRenderer->Initialize(size.x, size.y);

		RenderCommand::ClearColor(0.1f, 0.1f, 0.1f);

		float aspect = size.x / (float)size.y;
		mEditorCamera = EditorCamera(75.f, aspect, .01f, 1000.f);

		AddSubSystem<SelectionSubSystem>();
		AddSubSystem<ThumbnailCache>();
		AddSubSystem<UndoRedo>().init(5);

		auto &window_system = AddSubSystem<WindowSubSystem>();
		window_system.CreateWindow<LogPanel>();
		window_system.CreateWindow<HistoryWindow>();

		mSceneHeirarchyPanel = window_system.CreateWindow<SceneHierarchyPanel>();

		CreateWorld();

		mAssetManager = CreateRef<EditorAssetManager>(Project::GetResourceDirectory(), "AssetRegistry.json");
		AssetManager::SetAssetManager(mAssetManager.get());

		mContentBrowser =
			window_system.CreateWindow<EditorContentBrowser<EditorAssetManager>>(Project::GetResourceDirectory());

		SetupDefaultCommands();

		Inspect::set_property_changed_callback(
			[](auto object, const auto &prop, auto var)
			{
				auto &undo_system = GetSubSystem<UndoRedo>();
				undo_system.add_history_command<FCommandProperty>(
					std::format("{} Property Changed", prop.get_name().data()), object, prop, var);
			});
	}

	void EditorLayer::OnDetach()
	{
		GetSubSystem<UndoRedo>().shutdown();
	}

	void EditorLayer::OnUpdate(float dt)
	{
		auto size = mRenderer->GetSize();
		if ((mViewportSize.x > 0.f && mViewportSize.y > 0.f) && (mViewportSize.x != size.x || mViewportSize.y != size.y))
		{
			mEditorCamera.Resize(mViewportSize.x, mViewportSize.y);
			mRenderer->Resize(mViewportSize.x, mViewportSize.y);
		}

		if (mViewportHovered)
			mEditorCamera.ProcessInput();

		mRenderer->Begin(&mEditorCamera, mEditorCamera.GetView());

		Renderer::SubmitCamera(mEditorCamera.GetProjection(), mEditorCamera.GetView().inverse());

		RenderCommand::Clear();

		Renderer::Begin();

		mActiveWorld->Update(dt);

		LineRenderer::DrawGrid(FGrid{.size = 40.f, .divisions = 20, .color = 0xffffffff, .stepcolor = 0xff808080});

		Renderer::End();

		mRenderer->End();
	}

	void EditorLayer::OnEvent(Event &e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch(this, &EditorLayer::OnWindowResize);
		dispatcher.Dispatch(this, &EditorLayer::OnKeyEvent);

		if (mViewportHovered)
			mEditorCamera.OnEvent(e);
	}

	void EditorLayer::OnGuiRender()
	{
		static bool scene_heirarchy_status = true;
		static bool content_browser_status = true;

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl + N"))
				{
					CreateWorld();
				}

				if (ImGui::MenuItem("Open", "Ctrl + O"))
				{
					LoadWorld();
				}

				if (ImGui::MenuItem("Save", "Ctrl + S"))
				{
					SaveWorld();
				}

				if (ImGui::MenuItem("SaveAs...", "Ctrl + Shift + S"))
				{
					SaveWorldAs();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Windows"))
			{
				ImGui::Checkbox("Scene Hierarchy", &scene_heirarchy_status);
				ImGui::Checkbox("Content Browser", &content_browser_status);
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		Viewport();

		if (ImGui::Begin("Settings"))
		{

			Inspect::inspect("Renderer", mRenderer);
		}

		ImGui::End();

		if (ImGui::Begin("Assets"))
		{
			const auto &registry = mAssetManager->GetAssetRegistry();
			Inspect::inspect("", registry);
		}

		ImGui::End();

		ImGui::ShowDemoWindow();

		SubSystemContext::Get().GetSubSystem<WindowSubSystem>().UpdateWindows();
	}

	void EditorLayer::SetupDefaultCommands()
	{
		mCommands.emplace(FCommand{Key::Q}, [&]() { mGizmoOperation = 0; });
		mCommands.emplace(FCommand{Key::W}, [&]() { mGizmoOperation = ImGuizmo::TRANSLATE; });
		mCommands.emplace(FCommand{Key::E}, [&]() { mGizmoOperation = ImGuizmo::ROTATE; });
		mCommands.emplace(FCommand{Key::R}, [&]() { mGizmoOperation = ImGuizmo::SCALE; });
		mCommands.emplace(FCommand{Key::T}, [&]() { mGizmoOperation = ImGuizmo::UNIVERSAL; });
		mCommands.emplace(FCommand{Key::B}, [&]() { mGizmoOperation = ImGuizmo::BOUNDS; });
		mCommands.emplace(FCommand{Key::L}, [&]() { mGizmoMode = ImGuizmo::LOCAL; });
		mCommands.emplace(FCommand{Key::K}, [&]() { mGizmoMode = ImGuizmo::WORLD; });

		mCommands.emplace(FCommand{Key::O, Mod::Control}, [&]() { LoadWorld(); });
		mCommands.emplace(FCommand{Key::N, Mod::Control}, [&]() { CreateWorld(); });
		mCommands.emplace(FCommand{Key::S, Mod::Control}, [&]() { SaveWorld(); });
		mCommands.emplace(FCommand{Key::S, Mod::Control_Shift}, [&]() { SaveWorldAs(); });
		mCommands.emplace(FCommand{Key::Z, Mod::Control}, []() { GetSubSystem<UndoRedo>().undo(); });
		mCommands.emplace(FCommand{Key::Y, Mod::Control}, []() { GetSubSystem<UndoRedo>().redo(); });
	}

	bool EditorLayer::OnWindowResize(WindowResizeEvent &e)
	{

		return false;
	}

	bool EditorLayer::OnKeyEvent(KeyEvent &e)
	{
		if (mViewportHovered || mViewportFocused)
		{
			if (e.Action == EventStatus::PRESS)
			{
				FCommand command_code = {e.Key, e.Mods};
				if (mCommands.contains(command_code))
				{
					auto &command = mCommands.at(command_code);
					command();

					return true;
				}
			}
		}
		return false;
	}

	void EditorLayer::CreateWorld()
	{
		SubSystemContext::Get().GetSubSystem<SelectionSubSystem>().Clear();

		mEditorWorld = CreateRef<World>();
		mEditorWorld->SetName("New World");
		SetActiveWorld(mEditorWorld);

		mCurrentWorldPath = "";
		auto &window = Application::Get().GetWindow();
		window.SetTitle("New World");
	}

	void EditorLayer::SaveWorld()
	{
		if (mCurrentWorldPath.empty())
		{
			SaveWorldAs();
			return;
		}

		AssetFactory::Export(mEditorWorld, mCurrentWorldPath);

		auto &window = Application::Get().GetWindow();
		window.SetTitle(mCurrentWorldPath.stem().string());
	}

	void EditorLayer::SaveWorldAs()
	{
		auto path = FileDialogs::SaveFile(AssetFactory::GetFileFilters());
		if (path.empty())
			return;

		AssetFactory::Export(mEditorWorld, path);
		mCurrentWorldPath = path;
		mAssetManager->ImportAsset(mCurrentWorldPath, rttr::type::get<World>(), mEditorWorld->GetHandle());
	}

	void EditorLayer::LoadWorld()
	{
		auto path = FileDialogs::OpenFile(AssetFactory::GetFileFilters());
		if (path.empty())
			return;

		Ref<Asset> asset = mEditorWorld;
		AssetFactory::Import(asset, path);

		mEditorWorld = Cast<World>(asset);
		SetActiveWorld(mEditorWorld);
		mCurrentWorldPath = path;

		auto &window = Application::Get().GetWindow();
		window.SetTitle(mCurrentWorldPath.stem().string());
	}

	void EditorLayer::SetActiveWorld(const Ref<World> &world)
	{
		mActiveWorld = world;
		mSceneHeirarchyPanel->SetContext(mActiveWorld);
	}

	void EditorLayer::Viewport()
	{
		static bool viewport_status = true;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});

		if (ImGui::Begin("Viewport", &viewport_status))
		{

			bool shown = ImGui::BeginChild("Viewport##Image", {}, 0, ImGuiWindowFlags_MenuBar);

			if (shown)
			{
				// menu bar
				ViewportGUI();

				auto size = ImGui::GetContentRegionAvail();
				auto viewport_min_region = ImGui::GetWindowContentRegionMin();
				auto viewport_max_region = ImGui::GetWindowContentRegionMax();
				auto viewport_offset = ImGui::GetWindowPos();
				mViewportHovered = ImGui::IsWindowHovered();
				mViewportFocused = ImGui::IsWindowFocused();
				Application::Get().GetImGuiLayer().BlockEvents(!(mViewportHovered && mViewportFocused));

				mViewportSize = {size.x, size.y};
				mViewportBounds[0] = {viewport_min_region.x + viewport_offset.x, viewport_min_region.y + viewport_offset.y};
				mViewportBounds[1] = {viewport_max_region.x + viewport_offset.x, viewport_max_region.y + viewport_offset.y};

				glm::mat4 view = mEditorCamera.GetView().inverse();
				const glm::mat4 projection = mEditorCamera.GetProjection();

				auto color_attachment = mRenderer->GetColorAttachment();
				ImGui::Image((ImTextureID)(uint64_t)*color_attachment, size, {0, 1}, {1, 0});

				auto &selection = SubSystemContext::Get().GetSubSystem<SelectionSubSystem>();
				auto selected_object = selection.GetSelection();

				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(
					mViewportBounds[0].x, mViewportBounds[0].y, mViewportBounds[1].x - mViewportBounds[0].x,
					mViewportBounds[1].y - mViewportBounds[0].y);

				if (selected_object && mGizmoOperation != -1 && !mActiveWorld->IsRunning())
				{

					glm::mat4 local_transform = selected_object->GetLocalTransform().to_mat4();
					glm::mat4 world_transform = selected_object->GetWorldTransform();

					float snap_value = mSnappingEnabled ? sSnapValues[(ImGuizmo::OPERATION)mGizmoOperation] : 0.0f;
					float snap_values[3] = {snap_value, snap_value, snap_value};

					glm::mat4 delta{1.f};

					ImGuizmo::Manipulate(
						&view[0][0], &projection[0][0], (ImGuizmo::OPERATION)mGizmoOperation, (ImGuizmo::MODE)mGizmoMode,
						&local_transform[0][0], &delta[0][0], snap_values);

					if (ImGuizmo::IsUsing())
					{
						glm::mat4 parent_transform = glm::inverse(world_transform) * local_transform * delta;
						glm::mat4 new_transform = glm::inverse(parent_transform) * world_transform;

						selected_object->SetLocalTransform(local_transform);
					}
				}

				auto view_size = 100.0f;
				auto view_position = ImVec2(mViewportBounds[1].x - view_size, mViewportBounds[0].y);
				ImGuizmo::ViewManipulate(
					&view[0][0], mEditorCamera.Distance(), view_position, {view_size, view_size}, 0x00000000);

				if (ImGuizmo::IsUsingViewManipulate())
				{
					mEditorCamera.SetView(glm::inverse(view));
				}

				ImGui::EndChild();
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::ViewportGUI()
	{
		auto &thumbnail_cache = GetSubSystem<ThumbnailCache>();

		// icons
		auto play_icon = thumbnail_cache.Get("PlayIcon");
		auto stop_icon = thumbnail_cache.Get("StopIcon");
		auto pause = thumbnail_cache.Get("PauseIcon");
		auto step = thumbnail_cache.Get("StepIcon");

		const bool running = mActiveWorld->IsRunning();
		const bool paused = mActiveWorld->IsPaused();
		auto icon = running ? stop_icon : play_icon;
		auto pause_icon = !paused ? pause : play_icon;
		const auto icon_size = ImGui::GetLineHeight() * .75f;
		auto spacing = ImGui::GetStyle().ItemSpacing.x;
		const int num_buttons = 3;

		bool opened = ImGui::BeginMenuBar();

		if (opened)
		{
			auto width = ImGui::GetContentRegionAvail().x;

			if (ImGui::Button("Add New"))
			{
				ImGui::OpenPopup("ADD_NEW_GAMEOBJECT_MENU");
			}

			if (ImGui::BeginPopup("ADD_NEW_GAMEOBJECT_MENU"))
			{
				ImGui::EndPopup();
			}

			ImGui::RadioButton("Local", &mGizmoMode, ImGuizmo::LOCAL);
			ImGui::RadioButton("World", &mGizmoMode, ImGuizmo::WORLD);

			ImGui::SetCursorPosX((width * .5f) - (3 * icon_size + spacing * 3.f));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);

			if (ImGui::ImageButton("Viewport##PlayIcon", (ImTextureID)(uint64_t)*icon, {icon_size}, {0, 1}, {1, 0}))
			{
				SubSystemContext::Get().GetSubSystem<SelectionSubSystem>().Clear();
				if (running)
				{

					mActiveWorld->End();
					mActiveWorld = mEditorWorld;
					mSceneHeirarchyPanel->SetContext(mActiveWorld);
				}
				else
				{
					mActiveWorld = mEditorWorld->Copy();
					mSceneHeirarchyPanel->SetContext(mActiveWorld);
					mActiveWorld->Begin();
				}
			}

			ImGui::SameLine();

			ImGui::BeginDisabled(!running);
			if (ImGui::ImageButton("Viewport##PauseIcon", (ImTextureID)(uint64_t)*pause_icon, {icon_size}, {0, 1}, {1, 0}))
			{
				mActiveWorld->SetPaused(!paused);
			}

			ImGui::SameLine();

			if (ImGui::ImageButton("Viewport##StepIcon", (ImTextureID)(uint64_t)*step, {icon_size}, {0, 1}, {1, 0}))
			{
				mActiveWorld->Step();
			}

			ImGui::EndDisabled();
			ImGui::PopStyleVar();
		}

		ImGui::EndMenuBar();
	}

} // namespace BHive