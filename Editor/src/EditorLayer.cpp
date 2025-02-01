#include "asset/AssetManager.h"
#include "asset/EditorAssetManager.h"
#include "assetmenus/SpriteAssetContextMenu.h"
#include "core/FileDialog.h"
#include "core/WindowInput.h"
#include "DetailsPanel.h"
#include "EditorContentBrowser.h"
#include "EditorLayer.h"
#include "gui/Gui.h"
#include "inspector/Inspectors.h"
#include "project/Project.h"
#include "PropertiesPanel.h"
#include "scene/ITransform.h"
#include "scene/SceneRenderer.h"
#include "scene/World.h"
#include "SceneHeirarchyPanel.h"
#include "serialization/Serialization.h"
#include "subsystem/SubSystem.h"
#include "subsystems/EditSubSystem.h"
#include "subsystems/WindowSubSystem.h"
#include "windows/LogPanel.h"
#include "math/TransformCoordinates.hpp"
#include "scene/Entity.h"
#include "factories/entity_factories/EntityFactory.h"
#include "math/RayCasting.h"
#include "core/FPSCounter.h"
#include <ImGuizmo.h>
#include <mini/ini.h>
#include <rttr/library.h>

namespace BHive
{

	struct ImGuizmoSnap
	{
		ImGuizmoSnap()
		{
			mSnapping[ImGuizmo::OPERATION::TRANSLATE] = 10.f;
			mSnapping[ImGuizmo::OPERATION::ROTATE] = 45.0f;
			mSnapping[ImGuizmo::OPERATION::SCALE] = 0.1f;
		}

		float &operator[](int32_t key) { return mSnapping[(ImGuizmo::OPERATION)key]; }

		std::unordered_map<ImGuizmo::OPERATION, float> mSnapping;
	};

	static FPSCounter sCounter;
	static ImGuizmoSnap sSnapping;

	void EditorLayer::OnAttach()
	{
		if (!Log::OnMessageLogged.is_bound())
		{
			mLogEventHandle = Log::OnMessageLogged.bind(&LogPanel::LogMessage);
		}

		auto &application = Application::Get();
		auto &window = application.GetWindow();

		auto &cmd = application.GetSpecification().CommandLine;
		if (cmd.Count > 1)
		{
			auto project_path = cmd.Args[1];
			Project::LoadProject(project_path);

			auto &proj_configuration = Project::GetConfiguration();
			rttr::library project_lib(proj_configuration.mName);
			if (project_lib.load())
			{
				LOG_INFO("Loaded Project : {}", proj_configuration.mName);
			}

			std::filesystem::current_path(proj_configuration.mProjectDirectory);
		}
		else
		{
			Project::New({});
		}

		mEditorWorld = CreateRef<World>();
		mActiveWorld = mEditorWorld;

		auto w = window.GetWidth();
		auto h = window.GetHeight();
		auto aspect = w / (float)h;

		mSceneRenderer = CreateRef<SceneRenderer>(w, h, ESceneRendererFlags_VisualizeColliders);
		mEditorCamera = EditorCamera(45.f, aspect, 0.1f, 1000.f);

		mSceneHierarchyPanel = CreateRef<SceneHierarchyPanel>();
		mSceneHierarchyPanel->SetContext(mActiveWorld);

		mPropertiesPanel = CreateRef<PropertiesPanel>();

		mDetailsPanel = CreateRef<DetailsPanel>();

		mContentBrowser = CreateRef<EditorContentBrowser>(Project::GetResourceDirectory());

		mAssetManager = CreateRef<EditorAssetManager>(Project::GetResourceDirectory() / "asset_registry.registry");
		AssetManager::SetAssetManager(mAssetManager.get());

		SubSystemContext::Get().AddSubSystem<EditSubSystem>();

		auto &window_system = SubSystemContext::Get().AddSubSystem<WindowSubSystem>();
		window_system.AddWindow<LogPanel>();

		auto &edit_system = SubSystemContext::Get().GetSubSystem<EditSubSystem>();
		edit_system.GetEditorMode.bind([=]() { return mEditorMode; });
		edit_system.GetActiveWorld.bind([=]() { return mActiveWorld; });

		/*AnimGraphContextMenu menu;
		menu.OnAssetOpen(1202670764);*/
	}

	void EditorLayer::OnDetach()
	{
		auto &window_system = SubSystemContext::Get().AddSubSystem<WindowSubSystem>();
		window_system.Clear();

		if (Log::OnMessageLogged.is_bound())
		{
			Log::OnMessageLogged.unbind(mLogEventHandle);
		}
	}

	void EditorLayer::OnEvent(Event &event)
	{
		auto &window_system = SubSystemContext::Get().GetSubSystem<WindowSubSystem>();
		window_system.ProcessEvents(event);

		EventDispatcher dispatcher(event);

		if (mEditorMode != EEditorMode::PLAY)
		{
			if (mIsHovered)
			{
				mEditorCamera.OnEvent(event);
			}

			dispatcher.Dispatch(this, &EditorLayer::OnKeyEvent);
		}
	}

	void EditorLayer::OnUpdate(float dt)
	{
		sCounter.Begin();

		mEditorCamera.ProcessInput();

		const auto &size = mViewportPanelSize;
		if (((float)mViewportSize.x != size.x || (float)mViewportSize.y != size.y) && (size.x != 0 && size.y != 0))
		{
			mViewportSize = {(unsigned)size.x, (unsigned)size.y};
			mSceneRenderer->Resize(mViewportSize.x, mViewportSize.y);
			mEditorCamera.Resize(mViewportSize.x, mViewportSize.y);
			mActiveWorld->OnViewportResize(mViewportSize.x, mViewportSize.y);
		}

		if (mEditorMode != EEditorMode::PLAY)
		{
			mSceneRenderer->Begin(mEditorCamera, mEditorCamera.GetView().inverse());

			mSceneRenderer->SubmitRenderQueue(
				[]()
				{
					LineRenderer::DrawLine({}, {0, 10, 0}, 0xFF00FFFF);
					LineRenderer::DrawGrid({});
				});
		}

		mSceneRenderer->SubmitRenderQueue(
			[dt]()
			{
				auto &edit_system = SubSystemContext::Get().GetSubSystem<EditSubSystem>();
				edit_system.OnUpdate(dt);
			});

		switch (mEditorMode)
		{
		case EDIT:
		{
			mActiveWorld->UpdateEditor(mSceneRenderer);
			break;
		}
		case SIMULATE:
		{
			mActiveWorld->OnSimulate(dt, mSceneRenderer);
			break;
		}
		case PLAY:
		{
			mActiveWorld->OnRuntimeUpdate(dt, mSceneRenderer);
			break;
		}
		default:
			break;
		}

		if (mEditorMode != EEditorMode::PLAY)
		{
			mSceneRenderer->End();
		}

		sCounter.End();
	}

	void EditorLayer::OnGuiRender(float)
	{
		ImGuizmo::BeginFrame();

		GUI::BeginDockSpace("EditorDockspace");

		if (ImGui::BeginMenuBar())
		{
			FileMenu();
			WindowMenu();

			ImGui::EndMenuBar();
		}

		auto &window_system = SubSystemContext::Get().GetSubSystem<WindowSubSystem>();
		window_system.UpdateWindows();

		ShowSceneHierarchyPanel();
		ShowPropertiesPanel();
		ShowDetailsPanel();
		ShowViewport();
		ShowPerformancePanel();
		ShowContentBrowser();
		ShowAssetManagerPanel();
		ShowRenderSettings();
		ShowInputWindow();
		ShowProjectWindow();
		ShowWorldSettings();

		GUI::EndDockSpace();
	}

	bool EditorLayer::OnKeyEvent(KeyEvent &event)
	{
		bool ctrl = (event.Mods & Mod::Control) != 0;
		bool shift = (event.Mods & Mod::Shift) != 0;

		mSnapEnabled = ctrl;

		// handle viewport events
		if (mIsFocused)
		{
			switch (event.Key)
			{
			case Key::W:
			{
				mGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
				return true;
			}
			case Key::E:
			{
				mGizmoOperation = ImGuizmo::OPERATION::ROTATE;
				return true;
			}
			case Key::R:
			{
				mGizmoOperation = ImGuizmo::OPERATION::SCALE;
				return true;
			}
			case Key::Q:
			{
				mGizmoOperation = -1;
				return true;
			}
			case Key::T:
			{
				mGizmoOperation = ImGuizmo::OPERATION::UNIVERSAL;
				return true;
			}
			case Key::L:
			{
				mGizmoMode = ImGuizmo::MODE::LOCAL;
				return true;
			}
			case Key::K:
			{
				mGizmoMode = ImGuizmo::MODE::WORLD;
				return true;
			}
			}
		}

		switch (event.Key)
		{
		case Key::N:
		{
			if (ctrl)
			{
				NewWorld();
				return true;
			}

			break;
		}
		case Key::O:
		{
			if (ctrl)
			{
				OpenWorld();
				return true;
			}

			break;
		}
		case Key::S:
		{
			if (ctrl)
			{
				if (shift)
				{
					SaveWorldAs();
					return true;
				}
				else
				{
					SaveWorld();
					return true;
				}
			}

			break;
		}
		default:
			break;
		}

		return false;
	}

	void EditorLayer::FileMenu()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New", "Ctrl + N"))
			{
				NewWorld();
			}
			if (ImGui::MenuItem("Open", "Ctrl + O"))
			{
				OpenWorld();
			}
			if (ImGui::MenuItem("Save", "Ctrl + S"))
			{
				SaveWorld();
			}
			if (ImGui::MenuItem("SaveAs...", "Ctrl + Shift + S"))
			{
				SaveWorldAs();
			}
			if (ImGui::MenuItem("Exit"))
			{
				Application::Get().Close();
			}
			ImGui::EndMenu();
		}
	}

	void EditorLayer::WindowMenu()
	{
		if (ImGui::BeginMenu("Windows"))
		{
			auto &window_system = SubSystemContext::Get().GetSubSystem<WindowSubSystem>();

			if (ImGui::MenuItem("Asset Manager", "", &mShowAssetManagerAssets))
			{
				mShowAssetManagerAssets = true;
			}

			if (ImGui::MenuItem("Render Settings", "", &mShowRenderSettings))
			{
				mShowRenderSettings = true;
			}

			if (ImGui::MenuItem("Input Manager", "", &mShowInputSettings))
			{
				mShowInputSettings = true;
			}

			if (ImGui::MenuItem("World Settings", "", &mShowWorldSettings))
			{
				mShowWorldSettings = true;
			}

			if (ImGui::MenuItem("Log Window", ""))
			{
				window_system.AddWindow<LogPanel>();
			}

			if (ImGui::MenuItem("Project Window", "", &mShowProjectWindow))
			{
				mShowProjectWindow = true;
			}

			ImGui::EndMenu();
		}
	}

	void EditorLayer::ShowSceneHierarchyPanel()
	{
		static bool opened = true;

		if (ImGui::Begin("Scene Hierarchy", &opened))
		{
			mSceneHierarchyPanel->OnGuiRender();
		}

		ImGui::End();
	}

	void EditorLayer::ShowPropertiesPanel()
	{
		static bool opened = true;
		if (ImGui::Begin("PropertiesPanel", &opened))
		{
			mPropertiesPanel->OnGuiRender();
		}

		ImGui::End();
	}

	void EditorLayer::ShowDetailsPanel()
	{
		mDetailsPanel->OnGuiRender();
	}

	void EditorLayer::ShowViewport()
	{
		auto &edit_system = SubSystemContext::Get().GetSubSystem<EditSubSystem>();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
		if (ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_MenuBar))
		{

			ViewportToolbar();

			mIsHovered = ImGui::IsWindowHovered();
			mIsFocused = ImGui::IsWindowFocused();

			Application::Get().GetImGuiLayer().BlockEvents(!mIsHovered && !mIsFocused);

			auto viewport_min_region = ImGui::GetWindowContentRegionMin();
			auto viewport_max_region = ImGui::GetWindowContentRegionMax();
			auto viewport_offset = ImGui::GetWindowPos();
			mViewportPanelSize = ImGui::GetContentRegionAvail();

			mViewportBounds[0] = viewport_min_region + viewport_offset;
			mViewportBounds[1] = viewport_max_region + viewport_offset;

			auto fbo_texture = mSceneRenderer->GetFinalFramebuffer()->GetColorAttachment()->GetRendererID();
			ImGui::Image((ImTextureID)(uint64_t)fbo_texture, mViewportPanelSize, {0, 1}, {1, 0});

			// DragDropAsset
			if (ImGui::BeginDragDropTarget())
			{
				auto payload = ImGui::AcceptDragDropPayload("ASSET");
				if (payload)
				{
					auto handle = *(AssetHandle *)payload->Data;
					auto &meta_data = mAssetManager->GetMetaData(handle);
					if (meta_data.Type.is_derived_from<World>())
					{
						OpenWorld(Project::GetResourceDirectory() / meta_data.Path);
					}
					else
					{
						const auto mouse_pos = ImGui::GetMousePos() - viewport_offset;
						const auto projection = mEditorCamera.GetProjection();
						const auto view = mEditorCamera.GetView();

						LOG_TRACE("Mouse Pos {}, {}", mouse_pos.x, mouse_pos.y);

						const auto mouse_ray =
							GetMouseRay(mouse_pos.x, mouse_pos.y, mViewportPanelSize.x, mViewportPanelSize.y, projection, view.inverse());

						FRay ray{view.get_translation(), mouse_ray};

						const float distance = RayCast::GetRayPlaneDistance(ray, {0, 1, 0}, 0.f);
						auto world_pos = RayCast::GetPointOnRay(view.get_translation(), mouse_ray, distance);

						LOG_TRACE("Mouse World Pos {}, World Pos Ray {}", mouse_ray, world_pos);

						if (auto factory = GetEntityFactory(meta_data))
						{
							auto new_entity = factory->CreateEntityFrom(meta_data, mActiveWorld.get(), world_pos);
							edit_system.mSelection.Select(&*new_entity);
						}
					}
				}

				ImGui::EndDragDropTarget();
			}

			auto current_selection = Cast<ITransform>(edit_system.mSelection.GetSelectedEntity());

			if (current_selection && mGizmoOperation != -1 && mEditorMode == EEditorMode::EDIT)
			{
				glm::mat4 transform = current_selection->GetWorldTransform();

				const glm::mat4 view = mEditorCamera.GetView().inverse();
				const glm::mat4 projection = mEditorCamera.GetProjection();

				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(
					mViewportBounds[0].x, mViewportBounds[0].y, mViewportBounds[1].x - mViewportBounds[0].x,
					mViewportBounds[1].y - mViewportBounds[0].y);

				float snap_value = mSnapEnabled ? sSnapping[(ImGuizmo::OPERATION)mGizmoOperation] : 0.0f;
				float snap_values[3] = {snap_value, snap_value, snap_value};

				if (ImGuizmo::Manipulate(
						glm::value_ptr(view), glm::value_ptr(projection), (ImGuizmo::OPERATION)mGizmoOperation, (ImGuizmo::MODE)mGizmoMode,
						glm::value_ptr(transform), nullptr, snap_values))
				{

					current_selection->SetWorldTransform(transform);
				}
			}
		}

		ImGui::End();

		ImGui::PopStyleVar();
	}

	void EditorLayer::ShowPerformancePanel()
	{
		if (ImGui::Begin("Performance"))
		{
			ImGui::Text("FPS: %.4f f/s", (float)sCounter);
		}

		ImGui::End();
	}

	void EditorLayer::ShowContentBrowser()
	{
		static bool opened = true;
		if (ImGui::Begin("Content Browser", &opened, ImGuiWindowFlags_MenuBar))
		{
			mContentBrowser->OnGuiRender();
		}

		ImGui::End();
	}

	void EditorLayer::ShowAssetManagerPanel()
	{
		if (!mShowAssetManagerAssets)
			return;

		if (ImGui::Begin("Asset Manager", &mShowAssetManagerAssets))
		{
			auto manager = AssetManager::GetAssetManager<EditorAssetManager>();
			auto &assets = manager->GetAssetRegistry();

			for (auto &[handle, metadata] : assets)
			{
				rttr::variant metadata_var = metadata;

				ImGui::BeginDisabled();
				inspect("Handle", handle, false, true);
				inspect(metadata_var, false, true);
				ImGui::EndDisabled();

				ImGui::Separator();
			}
		}

		ImGui::End();
	}

	void EditorLayer::ShowRenderSettings()
	{
		if (!mShowRenderSettings)
			return;

		if (ImGui::Begin("RenderSettings", &mShowRenderSettings))
		{
			rttr::variant var = mSceneRenderer.get();
			if (inspect(var))
			{
				auto ptr = mSceneRenderer.get();
				ptr = var.get_value<SceneRenderer *>();
			}
		}

		ImGui::End();
	}

	void EditorLayer::ShowInputWindow()
	{
		if (!mShowInputSettings)
			return;

		if (ImGui::Begin("InputManager", &mShowInputSettings))
		{
		}

		ImGui::End();
	}

	void EditorLayer::ShowProjectWindow()
	{
		if (!mShowProjectWindow)
			return;

		if (ImGui::Begin("Project", &mShowProjectWindow))
		{
			auto project_ptr = &Project::GetConfiguration();
			rttr::variant project_var = project_ptr;
			if (inspect(project_var))
			{
				project_ptr = project_var.get_value<FProjectConfiguration *>();
			}

			if (ImGui::Button("Create"))
			{
				auto path_str = FileDialogs::SaveFile("Project (*.proj)\0*.proj\0");
				if (!path_str.empty())
					Project::SaveProject(path_str);
			}
		}

		ImGui::End();
	}

	void EditorLayer::ShowWorldSettings()
	{
		if (ImGui::Begin("World Settings", &mShowWorldSettings))
		{
			if (mActiveWorld)
			{
				auto ptr = mActiveWorld.get();
				rttr::variant var = ptr;
				if (inspect(var))
				{
					ptr = var.get_value<World *>();
				}
			}
		}

		ImGui::End();
	}

	void EditorLayer::ViewportToolbar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (mEditorMode == EEditorMode::EDIT)
			{
				if (ImGui::MenuItem("Play"))
				{
					OnWorldPlay();
				}

				if (ImGui::MenuItem("Simulate"))
				{
					OnWorldSimulate();
				}
			}

			if (mEditorMode != EEditorMode::EDIT)
			{
				if (ImGui::MenuItem("Stop"))
				{
					OnWorldStop();
				}

				auto paused = mActiveWorld->IsPaused();
				if (ImGui::MenuItem(paused ? "Play" : "Pause"))
				{

					mActiveWorld->SetPaused(!paused);
				}

				if (paused)
				{
					if (ImGui::MenuItem("Step"))
					{
						mActiveWorld->Step();
					}
				}
			}

			ImGui::EndMenuBar();
		}
	}

	void EditorLayer::NewWorld()
	{
		if (mEditorMode != EEditorMode::EDIT)
			return;

		auto &edit_system = SubSystemContext::Get().GetSubSystem<EditSubSystem>();
		edit_system.mSelection.Clear();

		mEditorWorld = CreateRef<World>();
		mActiveWorld = mEditorWorld;

		mSceneHierarchyPanel->SetContext(mActiveWorld);
		mCurrentWorldPath.clear();

		LOG_TRACE("Create New World");
	}

	void EditorLayer::SaveWorld()
	{
		if (mCurrentWorldPath.empty())
			SaveWorldAs();

		std::ofstream out(mCurrentWorldPath, std::ios::out | std::ios::binary);
		if (!out)
			return;

		cereal::BinaryOutputArchive ar(out);
		mEditorWorld->Save(ar);

		LOG_TRACE("Saved World {}", mCurrentWorldPath.string());
		mAssetManager->ImportAsset(mCurrentWorldPath, rttr::type::get<World>(), mEditorWorld->GetHandle());

		SaveEditorSettings();
	}

	void EditorLayer::SaveWorldAs()
	{
		auto path = FileDialogs::SaveFile("World (.*world)\0*.world\0");
		if (path.empty())
			return;

		mCurrentWorldPath = path;
		SaveWorld();
	}

	void EditorLayer::OpenWorld(const std::filesystem::path &path)
	{
		if (mEditorMode != EEditorMode::EDIT)
			return;

		std::filesystem::path open_path = path;
		if (open_path.empty())
		{
			auto path_str = FileDialogs::OpenFile("World (*.world)\0*.world\0");
			if (path_str.empty())
				return;

			open_path = path_str;
		}

		auto &edit_system = SubSystemContext::Get().GetSubSystem<EditSubSystem>();
		edit_system.mSelection.Clear();

		mEditorWorld = CreateRef<World>();

		std::ifstream in(open_path, std::ios::in | std::ios::binary);
		if (!in)
			return;

		cereal::BinaryInputArchive ar(in);
		mEditorWorld->Load(ar);

		mActiveWorld = mEditorWorld;
		mSceneHierarchyPanel->SetContext(mActiveWorld);
		mCurrentWorldPath = open_path;

		LOG_TRACE("Opened World {}", mCurrentWorldPath.string());

		LoadEditorSettings();
	}

	void EditorLayer::OnWorldPlay()
	{
		if (mEditorMode == EEditorMode::SIMULATE)
			OnWorldStop();

		mEditorMode = EEditorMode::PLAY;

		mActiveWorld = mEditorWorld->Copy();
		mActiveWorld->OnRuntimeStart();

		mSceneHierarchyPanel->SetContext(mActiveWorld);

		auto &edit_system = SubSystemContext::Get().GetSubSystem<EditSubSystem>();
		edit_system.mSelection.Clear();
	}

	void EditorLayer::OnWorldStop()
	{
		ASSERT(mEditorMode == EEditorMode::PLAY || mEditorMode == EEditorMode::SIMULATE);

		switch (mEditorMode)
		{
		case SIMULATE:
		{
			mActiveWorld->OnSimulateStop();
			break;
		}
		case PLAY:
		{
			mActiveWorld->OnRuntimeStop();
			break;
		}
		}

		mEditorMode = EEditorMode::EDIT;

		mActiveWorld = mEditorWorld;

		mSceneHierarchyPanel->SetContext(mActiveWorld);

		auto &edit_system = SubSystemContext::Get().GetSubSystem<EditSubSystem>();
		edit_system.mSelection.Clear();
	}

	void EditorLayer::OnWorldSimulate()
	{
		if (mEditorMode == EEditorMode::PLAY)
			OnWorldStop();

		mEditorMode = EEditorMode::SIMULATE;

		mActiveWorld = mEditorWorld->Copy();
		mActiveWorld->OnSimulateStart();

		mSceneHierarchyPanel->SetContext(mActiveWorld);

		auto &edit_system = SubSystemContext::Get().GetSubSystem<EditSubSystem>();
		edit_system.mSelection.Clear();
	}

	void EditorLayer::SaveEditorSettings()
	{
		mINI::INIStructure ini;

		std::stringstream ss(std::stringstream::out);
		ss << mEditorCamera.GetView();
		ini["Editor"]["Camera"] = ss.str();

		auto path = mCurrentWorldPath.parent_path() / (mCurrentWorldPath.stem().string() + ".ini");
		mINI::INIFile file(path.string());
		if (!file.write(ini))
		{
			LOG_ERROR("EditorLayer::SaveEditorSettings: failed to save {}", path.string());
		}
	}

	void EditorLayer::LoadEditorSettings()
	{
		auto path = mCurrentWorldPath.parent_path() / (mCurrentWorldPath.stem().string() + ".ini");
		if (!std::filesystem::exists(path))
		{
			return;
		}

		mINI::INIStructure ini;

		mINI::INIFile file(path.string());
		if (!file.read(ini))
		{
			LOG_ERROR("EditorLayer::SaveEditorSettings: failed to read {}", path.string());
		}

		auto &camera_view = ini["Editor"]["Camera"];

		FTransform transform;

		std::stringstream ss(camera_view, std::stringstream::in);
		ss >> transform;

		mEditorCamera.SetView(transform);
	}

} // namespace BHive
