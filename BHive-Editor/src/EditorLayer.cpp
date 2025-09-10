#include "asset/AssetFactory.h"
#include "contextmenus/ContextMenus.h"
#include "core/Application.h"
#include "core/layers/ImGuiLayer.h"
#include "core/math/MathFunctionLibrary.h"
#include "core/math/RayCasting.h"
#include "core/platform/Platform.h"
#include "core/subsystem/SubSystem.h"
#include "core/Window.h"
#include "dragdropfactories/DragDropFactory.h"
#include "EditorLayer.h"
#include "GUI/Gui.h"
#include "gui/ImGuiExtended.h"
#include "gui/PayloadHelpers.h"
#include "ImGuizmo.h"
#include "inspectors/Inspect.h"
#include "project/Project.h"
#include "renderers/Renderer.h"
#include "renderers/EditorSceneRenderer.h"
#include "subsystems/Selection.h"
#include "undoredo/Commands.h"
#include "undoredo/UndoRedo.h"
#include "world/GameObject.h"
#include "world/World.h"
// windows
#include "windows/AssetWindow.h"
#include "windows/ContentBrowserWindow.h"
#include "windows/HistoryWindow.h"
#include "windows/ImWindowSystem.h"
#include "windows/LogWindow.h"
#include "windows/SceneHeirarchyWindow.h"
#include "renderers/render_passes/PickerRenderPass.h"
#include "renderers/render_passes/OutlineRenderPass.h"

#include "gfx/utils/texture/ImageUtils.h"
#include "world/components/LightComponents.h"
#include "core/profiler/CPUGPUProfiler.h"
#include "gui/Gimzo.h"

namespace BHive
{
#define RENDER_SETTINGS_POPUP_NAME "RenderSettingsPopup"

	constexpr const char *cImguiLayoutFilter = "Layout (*ini)\0 *.ini\0";

	static struct FSelectedData
	{
		Ref<FMeshRenderData> RenderData;
		GameObject *Object = nullptr;
	} sSelectedRenderData;

	void EditorLayer::OnAttach()
	{

		mGizmo.SnapValues[ImGuizmo::TRANSLATE] = glm::vec3{10.f};
		mGizmo.SnapValues[ImGuizmo::ROTATE] = glm::vec3{15.f};
		mGizmo.SnapValues[ImGuizmo::SCALE] = glm::vec3{.25f};

		auto set_world_lamda = [&](const UUID &handle)
		{
			auto asset = AssetManager::GetAsset<World>(handle);

			if (asset)
			{

				mEditorWorld = asset;
				SetActiveWorld(asset);

				auto &meta_data = mAssetManager->GetMetaData(handle);
				mCurrentWorldPath = Project::GetResourceDirectory() / meta_data.Path;
			}
		};

		FWorldContentMenu::OnAssetOpenedEvent.bind(set_world_lamda);

		auto &window = Application::Get().GetWindow();
		auto &size = window.GetSize();

		InitRenderer(size);

		float aspect = size.x / (float)size.y;
		mEditorCamera = EditorCamera(45.f, aspect, 0.001f, 5000.f);

		AddSubSystem<Selection>();
		AddSubSystem<ThumbnailCache>();

		AddSubSystem<ImWindowSystem>().ConstructWindow<ImLogWindow>();

		SetupDefaultCommands();
		auto project = Project::GetActive();
		if (project)
		{
			OnProjectOpened();
		}

		if (!mActiveWorld)
		{
			CreateWorld();
		}

		LoadEditorConfigFile();
	}

	void EditorLayer::OnDetach()
	{

		SaveEditorConfigFile();
	}

	void EditorLayer::OnUpdate(float dt)
	{
		GPU_PROFILER_SCOPED("GPUUpdate")
		CPU_PROFILER_FUNCTION()

		if (!mActiveWorld)
			return;

		auto size = mRenderer->GetSize();
		if ((mViewportSize.x > 0.f && mViewportSize.y > 0.f) && (mViewportSize.x != size.x || mViewportSize.y != size.y))
		{
			mEditorCamera.Resize(mViewportSize.x, mViewportSize.y);
			mRenderer->Resize(mViewportSize);
		}

		if (mViewportHovered)
			mEditorCamera.ProcessInput();

		mRenderer->Begin(&mEditorCamera, mEditorCamera.GetView());

		mActiveWorld->Update(dt, mRenderer.get());

		mRenderer->SubmitCommand([=]() { LineRenderer::DrawGrid(mStyles.GridStyle); });

		auto &thumbnail_cache = GetSubSystem<ThumbnailCache>();
		auto &registry = mActiveWorld->GetRegistry();
		static auto icon = thumbnail_cache.GetAssetIcon("PointLight");
		static FQuadParams params{};
		params.Size = {5, 5};

		auto lambda = [=](const entt::entity &e, const PointLightComponent &component)
		{
			auto gameobject = mActiveWorld->GetGameObject((int32_t)e);
			auto transform = gameobject->GetWorldTransform();
			mRenderer->SubmitCommand([=]() { QuadRenderer::DrawBillboard(params, icon, transform, (int32_t)e); });
		};

		registry.view<PointLightComponent>().each(lambda);

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
		static std::unordered_map<const char *, bool> windows = {{"ProjectEditor", false}, {"StyleEditor", false}};

		auto &window_system = SubSystemContext::Get().GetSubSystem<ImWindowSystem>();

		GUI::BeginDockSpace("dockspace");

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

				if (ImGui::MenuItem("Open Project", "Ctrl + P"))
				{
					if (auto info = Platform::OpenFile(Project::GetFileFilter()))
					{
						OpenProject(info);
					}
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Layouts"))
			{
				if (ImGui::MenuItem("Save Layout"))
				{
					if (auto info = Platform::SaveFile(cImguiLayoutFilter))
					{
						ImGui::SaveIniSettingsToDisk(info.AsString().c_str());
					}
				}

				if (ImGui::MenuItem("Load Layout"))
				{
					if (auto info = Platform::OpenFile(cImguiLayoutFilter))
					{
						ImGui::LoadIniSettingsFromDisk(info.AsString().c_str());
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Windows"))
			{
				if (ImGui::MenuItem("Asset Manager"))
				{
					auto window = window_system.ConstructWindow<ImAssetWindow>();
					window->SetContext(mAssetManager);
				}

				if (ImGui::MenuItem("Profiier"))
				{
					window_system.ConstructWindow<ImProfilerWindow>();
				}

				if (ImGui::MenuItem("History"))
				{
					window_system.ConstructWindow<ImHistoryWindow>();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("Edit"))
				{
					windows["ProjectEditor"] = true;
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Renderer"))
			{
				Inspect::get().inspect("Renderer", this, mRenderer);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Editor Style"))
			{
				windows["StyleEditor"] = true;

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if (windows["StyleEditor"])
		{
			if (ImGui::Begin("StyleEditor", &windows["StyleEditor"]))
			{
				ImGui::SeparatorText("Grid");

				Inspect::get().inspect("Color", this, mStyles.GridStyle.color);
				Inspect::get().inspect("StepColor", this, mStyles.GridStyle.stepcolor);
				Inspect::get().inspect("Divisions", this, mStyles.GridStyle.divisions);
				Inspect::get().inspect("size", this, mStyles.GridStyle.size);

				ImGui::ShowStyleEditor();
			}

			ImGui::End();
		}

		if (windows["ProjectEditor"])
		{
			if (ImGui::Begin("ProjectEditor", &windows["ProjectEditor"], ImGuiWindowFlags_NoSavedSettings))
			{
				auto &config = Project::GetConfiguration();

				auto scene = AssetManager::GetAsset<World>(config.StartScene);
				if (Inspect::get().inspect("Start Scene", config, scene))
				{
					config.StartScene = scene->GetHandle();
					Project::SaveProject();
				}
			}

			ImGui::End();
		}

#if 0
		if (ImGui::Begin("PostProcesses", nullptr, ImGuiWindowFlags_NoSavedSettings))
		{
			auto &post_proccesses = mRenderer->GetPostProcessPasses();

			for (auto &process : post_proccesses)
			{
				bool enabled = process->IsEnabled();

				if (ImGui::Checkbox(process->GetName(), &enabled))
				{
					process->SetEnabled(enabled);
				}
			}
		}

		ImGui::End();

#endif
		Viewport();

		window_system.Update();

		GUI::EndDockSpace();
	}

	void EditorLayer::SetupDefaultCommands()
	{
		mCommands.emplace(FCommand{Key::Q}, [&]() { mGizmo.Operation = 0; });
		mCommands.emplace(FCommand{Key::W}, [&]() { mGizmo.Operation = ImGuizmo::TRANSLATE; });
		mCommands.emplace(FCommand{Key::E}, [&]() { mGizmo.Operation = ImGuizmo::ROTATE; });
		mCommands.emplace(FCommand{Key::R}, [&]() { mGizmo.Operation = ImGuizmo::SCALE; });
		mCommands.emplace(FCommand{Key::T}, [&]() { mGizmo.Operation = ImGuizmo::UNIVERSAL; });
		mCommands.emplace(FCommand{Key::B}, [&]() { mGizmo.Operation = ImGuizmo::BOUNDS; });
		mCommands.emplace(FCommand{Key::L}, [&]() { mGizmo.Mode = ImGuizmo::LOCAL; });
		mCommands.emplace(FCommand{Key::K}, [&]() { mGizmo.Mode = ImGuizmo::WORLD; });

		mCommands.emplace(FCommand{Key::O, Mod::Control}, [&]() { LoadWorld(); });
		mCommands.emplace(FCommand{Key::N, Mod::Control}, [&]() { CreateWorld(); });
		mCommands.emplace(FCommand{Key::S, Mod::Control}, [&]() { SaveWorld(); });
		mCommands.emplace(FCommand{Key::S, Mod::Control_Shift}, [&]() { SaveWorldAs(); });
		mCommands.emplace(FCommand{Key::Z, Mod::Control}, []() { GetSubSystem<UndoRedo>().Undo(); });
		mCommands.emplace(FCommand{Key::Y, Mod::Control}, []() { GetSubSystem<UndoRedo>().Redo(); });
		mCommands.emplace(
			FCommand{Key::F},
			[&]()
			{
				auto selection = GetSubSystem<Selection>().GetSelection();
				if (selection)
				{
					mEditorCamera.Focus(selection->GetWorldTransform(), selection->GetBounds().Max);
				}
			});
	}

	void EditorLayer::InitRenderer(const glm::uvec2 &size)
	{
		mRenderer = CreateRef<EditorSceneRenderer>();
		mRenderer->Init(size);
		mRenderer->OnEntitySelectedEvent.bind(
			[&](int32_t entity)
			{
				auto object = mActiveWorld->GetGameObject(entity);
				auto &selection = SubSystemContext::Get().GetSubSystem<Selection>();
				selection.Select(object.get());
			});
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

	void EditorLayer::ClearSelection()
	{
		SubSystemContext::Get().GetSubSystem<Selection>().Clear();
		mRenderer->ClearPicked();
	}

	void EditorLayer::DrawGimzos()
	{
		auto &selection = SubSystemContext::Get().GetSubSystem<Selection>();
		auto selected_object = selection.GetSelection();

		GizmoContext::SetOrthographic(mEditorCamera.GetProjectionType() != EProjectionType::Perspective);
		GizmoContext::SetRect(mViewportBounds[0].x, mViewportBounds[0].y, mViewportBounds[1].x - mViewportBounds[0].x, mViewportBounds[1].y - mViewportBounds[0].y);

		glm::mat4 view = mEditorCamera.GetView().Inverse();
		const glm::mat4 projection = mEditorCamera.GetProjection();

		if (selected_object && mGizmo.Operation != -1 && !mActiveWorld->IsRunning())
		{

			Gizmo gizmo{};
			gizmo.SetOperationAndMode(mGizmo.Operation, mGizmo.Mode);

			glm::mat4 world_transform = selected_object->GetWorldTransform();

			auto snap_value = mGizmo.IsSnappingEnabled ? mGizmo.SnapValues[(ImGuizmo::OPERATION)mGizmo.Operation] : glm::vec3{0.0f};

			glm::mat4 delta{1.0f};

			gizmo.Manipulate(view, projection, world_transform, delta, snap_value);

			if (gizmo)
			{
				glm::mat4 local_transform = delta * selected_object->GetLocalTransform().to_mat4();
				selected_object->SetLocalTransform(local_transform);
			}
		}

		auto view_size = 100.0f;
		auto view_position = ImVec2(mViewportBounds[1].x - view_size, mViewportBounds[0].y);
		ViewGizmo view_gizmo{};
		view_gizmo.ViewManipulate(view, mEditorCamera.Distance(), view_position, {view_size, view_size}, 0x00000000);

		if (view_gizmo)
		{
			mEditorCamera.SetView(glm::inverse(view));
		}
	}

	void EditorLayer::CreateWorld()
	{
		SubSystemContext::Get().GetSubSystem<Selection>().Clear();

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

		auto path = Project::GetProjectDirectory() / "snapshot.png";
		ImageUtils::SaveImage(path, mRenderer->GetFramebuffer(), 0);
	}

	void EditorLayer::SaveWorldAs()
	{
		if (auto info = Platform::SaveFile(AssetFactory::GetFileFilters()))
		{
			AssetFactory::Export(mEditorWorld, info);
			mCurrentWorldPath = info;
			mAssetManager->ImportAsset(mCurrentWorldPath, rttr::type::get<World>(), mEditorWorld->GetHandle());
		}
	}

	void EditorLayer::LoadWorld()
	{
		if (auto info = Platform::OpenFile(AssetFactory::GetFileFilters()))
		{
			LoadWorld(info);
		}
	}

	void EditorLayer::LoadWorld(const std::filesystem::path &path)
	{
		if (!AssetFactory::Import(mEditorWorld, path))
			return;

		SetActiveWorld(mEditorWorld);
		mCurrentWorldPath = path;
	}

	void EditorLayer::SetActiveWorld(const Ref<World> &world)
	{
		if (!world)
			return;

		ClearSelection();

		mActiveWorld = world;
		mSceneHeirarchyPanel->SetContext(mActiveWorld);

		auto &window = Application::Get().GetWindow();
		window.SetTitle(world->GetName());
	}

	bool EditorLayer::LoadProjectLibrary(const std::string &lib)
	{
		rttr::library project_lib(lib);
		if (project_lib.is_loaded())
			project_lib.unload();

		if (!project_lib.load())
		{
			LOG_ERROR("RTTR::ERROR - Library {}", project_lib.get_error_string());
			return false;
		}

		LOG_INFO("Plugin Library {} loaded", lib);
		return true;
	}

	void EditorLayer::OpenProject(const std::filesystem::path &path)
	{
		auto proj = Project::LoadProject(path);
		OnProjectOpened();
	}

	void EditorLayer::OnProjectOpened()
	{
		auto &window_system = GetSubSystem<ImWindowSystem>();
		auto project_name = Project::GetProjectName();
		const auto resource_directory = Project::GetResourceDirectory();
		const auto module_path = Project::GetModulePath();

		mContentBrowser = window_system.ConstructWindow<EditorContentBrowser<EditorAssetManager>>(resource_directory);
		mSceneHeirarchyPanel = window_system.ConstructWindow<ImSceneHierarchy>();
		mContentBrowser->SetBaseDirectory(resource_directory);
		mAssetManager = CreateRef<EditorAssetManager>(resource_directory, "AssetRegistry.json");
		AssetManager::SetAssetManager(mAssetManager.get());

		if (!LoadProjectLibrary(module_path.string()))
			return;

		// reload content browser directory

		const auto &config = Project::GetConfiguration();
		if (config.StartScene)
		{
			auto &meta_data = mAssetManager->GetMetaData(config.StartScene);
			LoadWorld(resource_directory / meta_data.Path);
		}
	}

	void EditorLayer::LoadEditorConfigFile()
	{
		auto config = Project::GetProjectDirectory() / "EditorConfig.json";
		if (std::filesystem::exists(config))
		{
			std::ifstream in(config, std::ios::in);
			cereal::JSONInputArchive ar(in);
			ar(*this);
		}

		auto layout = std::filesystem::path(EDITOR_RESOURCE_PATH) / ("layouts/" + mCurrentLayout.Name);
		ImGui::LoadIniSettingsFromDisk(layout.string().c_str());
	}

	void EditorLayer::SaveEditorConfigFile()
	{
		auto config = Project::GetProjectDirectory() / "EditorConfig.json";
		std::ofstream out(config, std::ios::out);
		cereal::JSONOutputArchive ar(out);
		ar(*this);
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

				auto color_attachment = mRenderer->GetColorAttachment();

				ImGui::Image((ImTextureID)(uint64_t)*color_attachment, size, {0, 1}, {1, 0});

				DrawGimzos();

				if (mViewportFocused || mViewportHovered)
				{

					if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !GizmoContext::IsUsing())
					{
						auto [mx, my] = ImGui::GetMousePos();
						mx -= mViewportBounds[0].x;
						my -= mViewportBounds[0].y;
						my = mViewportSize.y - my;
						int mouse_x = (int)mx;
						int mouse_y = (int)my;

						mRenderer->Pick({mouse_x, mouse_y});
					}
				}

				if (ImGui::BeginDragDropTarget())
				{
					if (auto payload = ImGui::AcceptDragDropPayload(DRAG_DROP_SOURCE_TYPE))
					{
						auto data = (const char *)payload->Data;
						auto entries = PayloadHelpers::GetEntriesFromBuffer(data, payload->DataSize);

						for (auto &entry : entries)
						{
							const auto &camera_view = mEditorCamera.GetView();
							const auto mouse_pos = ImGui::GetMousePos();
							auto mouse_ray = MathFunctionLibrary::GetMouseRay(mouse_pos.x, mouse_pos.y, mViewportSize.x, mViewportSize.y, mEditorCamera.GetProjection(), camera_view.Inverse());

							auto distance = glm::distance(camera_view[0], {0, 0, 0});
							auto point = RayCast::GetPointOnRay(mEditorCamera.GetView()[0], mouse_ray, distance);

							auto relative = std::filesystem::relative(entry, Project::GetResourceDirectory());
							auto metadata = mAssetManager->GetMetaData(relative);

							if (metadata)
							{

								auto handle = mAssetManager->GetHandle(metadata.Path);
								if (auto asset = mAssetManager->GetAsset(handle))
								{
									if (auto factory = DragDropFactory::GetFactoryFromType(metadata.Type); factory && factory->CanCreate(metadata.Type))
										factory->CreateFrom(asset, metadata.Name, mActiveWorld.get(), point);
								}
							}
						}
					}

					ImGui::EndDragDropTarget();
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

			ImGui::RadioButton("Local", &mGizmo.Mode, ImGuizmo::LOCAL);
			ImGui::RadioButton("World", &mGizmo.Mode, ImGuizmo::WORLD);

			ImGui::SetCursorPosX((width * .5f) - (3 * icon_size + spacing * 3.f));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);

			if (ImGui::ImageButton("Viewport##PlayIcon", (ImTextureID)(uint64_t)*icon, {icon_size}, {0, 1}, {1, 0}))
			{
				SubSystemContext::Get().GetSubSystem<Selection>().Clear();
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