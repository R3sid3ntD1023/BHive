#include "EditorLayer.h"
#include "GUI/Gui.h"
#include "gfx/Framebuffer.h"
#include "core/Application.h"
#include "core/Window.h"
#include "core/layers/ImGuiLayer.h"
#include "gfx/RenderCommand.h"
#include "renderers/Renderer.h"
#include "editor/WindowSubSystem.h"
#include "editor/SelectionSubSystem.h"
#include "subsystem/SubSystem.h"
#include "Inspectors.h"
#include "objects/GameObject.h"
#include "editor/contextmenus/ContextMenus.h"
#include "asset/AssetFactory.h"
#include "core/FileDialog.h"
#include <ImGuizmo.h>

namespace BHive
{
	std::unordered_map<uint8_t, float> sSnapValues = {
		{ImGuizmo::TRANSLATE, 10.f}, {ImGuizmo::ROTATE, 15.f}, {ImGuizmo::SCALE, .25f}};

	void EditorLayer::OnAttach()
	{
		FWorldContentMenu::OnAssetOpenedEvent.bind(
			[&](const UUID &handle)
			{
				auto asset = AssetManager::GetAsset<World>(handle);

				if (asset)
				{
					SubSystemContext::Get().GetSubSystem<SelectionSubSystem>().Clear();
					mEditorWorld = asset;
					mActiveWorld = mEditorWorld;
					mSceneHeirarchyPanel->SetContext(mActiveWorld);
				}
			});

		SetCurrentContext(ImGui::GetCurrentContext());

		auto &window = Application::Get().GetWindow();
		auto &size = window.GetSize();

		FramebufferSpecification specs{};
		specs.Width = size.x;
		specs.Height = size.y;
		specs.Attachments.attach({.InternalFormat = EFormat::RGBA8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});
		mFramebuffer = CreateRef<Framebuffer>(specs);

		mEditorWorld = CreateRef<World>();
		mEditorWorld->SetName("EditorWorld");
		mActiveWorld = mEditorWorld;

		RenderCommand::ClearColor(.2f, .2f, .2f);

		float aspect = size.x / (float)size.y;
		mEditorCamera = EditorCamera(75.f, aspect, .01f, 100.f);

		SubSystemContext::Get().AddSubSystem<SelectionSubSystem>();
		auto &window_system = SubSystemContext::Get().AddSubSystem<WindowSubSystem>();
		mSceneHeirarchyPanel = window_system.CreateWindow<SceneHierarchyPanel>();
		mSceneHeirarchyPanel->SetContext(mActiveWorld);
		mPropertiesPanel = window_system.CreateWindow<PropertiesPanel>();

		mContentBrowser = window_system.CreateWindow<EditorContentBrowser<EditorAssetManager>>(RESOURCE_PATH);

		FProjectConfiguration config{};
		config.mName = "Mario";
		config.mProjectDirectory = RESOURCE_PATH "../";
		config.mResourcesDirectory = "resources";
		Project::New(config);
		AssetManager::SetAssetManager(&mAssetManager);

		SetupDefaultCommands();
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(float dt)
	{
		auto w = mFramebuffer->GetWidth();
		auto h = mFramebuffer->GetHeight();
		if ((mViewportSize.x > 0.f && mViewportSize.y > 0.f) && (mViewportSize.x != w || mViewportSize.y != h))
		{
			mEditorCamera.Resize(mViewportSize.x, mViewportSize.y);
			mFramebuffer->Resize(mViewportSize.x, mViewportSize.y);
		}

		if (mViewportHovered)
			mEditorCamera.ProcessInput();

		Renderer::SubmitCamera(mEditorCamera.GetProjection(), mEditorCamera.GetView());

		mFramebuffer->Bind();

		RenderCommand::Clear();

		Renderer::Begin();

		mActiveWorld->Update(dt);

		LineRenderer::DrawGrid(FGrid{.color = 0x808080ff, .stepcolor = 0x808080ff});

		Renderer::End();

		mFramebuffer->UnBind();
	}

	void EditorLayer::OnEvent(Event &e)
	{
		if (mViewportHovered)
			mEditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch(this, &EditorLayer::OnWindowResize);
		dispatcher.Dispatch(this, &EditorLayer::OnKeyEvent);
	}

	void EditorLayer::OnGuiRender()
	{
		static bool viewport_status = true;
		static bool scene_heirarchy_status = true;
		static bool properties_status = true;
		static bool content_browser_status = true;

		GUI::BeginDockSpace("DockSpace");

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

				if (ImGui::MenuItem("SaveAs...", "Ctrl + Alt + S"))
				{
					SaveWorldAs();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		SubSystemContext::Get().GetSubSystem<WindowSubSystem>().UpdateWindows();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
		if (ImGui::Begin("Viewport", &viewport_status, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				auto label = mActiveWorld->IsRunning() ? "Stop" : "Play";
				if (ImGui::Button(label))
				{
					if (mActiveWorld->IsRunning())
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

				ImGui::EndMenuBar();
			}
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

			ImGui::Image((ImTextureID)(uint64_t)*mFramebuffer->GetColorAttachment(), size, {0, 1}, {1, 0});

			auto &selection = SubSystemContext::Get().GetSubSystem<SelectionSubSystem>();
			auto selected_object = selection.GetSelection();

			if (selected_object && mGizmoOperation != -1)
			{

				glm::mat4 world_transform = selected_object->GetTransform().to_mat4();
				glm::mat4 local_transform = selected_object->GetLocalTransform().to_mat4();
				glm::mat4 parent_transform = glm::inverse(world_transform) * local_transform;

				const glm::mat4 view = mEditorCamera.GetView();
				const glm::mat4 projection = mEditorCamera.GetProjection();

				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(
					mViewportBounds[0].x, mViewportBounds[0].y, mViewportBounds[1].x - mViewportBounds[0].x,
					mViewportBounds[1].y - mViewportBounds[0].y);

				float snap_value = mSnappingEnabled ? sSnapValues[(ImGuizmo::OPERATION)mGizmoOperation] : 0.0f;
				float snap_values[3] = {snap_value, snap_value, snap_value};

				glm::mat4 delta{1.f};
				if (ImGuizmo::Manipulate(
						&view[0][0], &projection[0][0], (ImGuizmo::OPERATION)mGizmoOperation, (ImGuizmo::MODE)mGizmoMode,
						&world_transform[0][0], &delta[0][0], snap_values))
				{
					glm::mat4 new_transform = glm::inverse(parent_transform) * world_transform;

					selected_object->SetTransform(new_transform);
				}
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();

		GUI::EndDockSpace();
	}

	void EditorLayer::SetupDefaultCommands()
	{
		mCommands.emplace(FCommand{Key::Q}, [&]() { mGizmoOperation = 0; });
		mCommands.emplace(FCommand{Key::W}, [&]() { mGizmoOperation = ImGuizmo::TRANSLATE; });
		mCommands.emplace(FCommand{Key::E}, [&]() { mGizmoOperation = ImGuizmo::ROTATE; });
		mCommands.emplace(FCommand{Key::R}, [&]() { mGizmoOperation = ImGuizmo::SCALE; });
		mCommands.emplace(FCommand{Key::T}, [&]() { mGizmoOperation = ImGuizmo::UNIVERSAL; });
		mCommands.emplace(FCommand{Key::L}, [&]() { mGizmoMode = ImGuizmo::LOCAL; });
		mCommands.emplace(FCommand{Key::K}, [&]() { mGizmoMode = ImGuizmo::WORLD; });

		mCommands.emplace(FCommand{Key::O, Mod::Control}, [&]() { LoadWorld(); });
		mCommands.emplace(FCommand{Key::N, Mod::Control}, [&]() { CreateWorld(); });
		mCommands.emplace(FCommand{Key::S, Mod::Control}, [&]() { SaveWorld(); });
		mCommands.emplace(FCommand{Key::S, Mod::Control | Mod::Alt}, [&]() { SaveWorldAs(); });
	}

	bool EditorLayer::OnWindowResize(WindowResizeEvent &e)
	{

		return false;
	}

	bool EditorLayer::OnKeyEvent(KeyEvent &e)
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
		return false;
	}

	void EditorLayer::CreateWorld()
	{
		SubSystemContext::Get().GetSubSystem<SelectionSubSystem>().Clear();

		mEditorWorld = CreateRef<World>();
		mActiveWorld = mEditorWorld;
		mSceneHeirarchyPanel->SetContext(mActiveWorld);

		mCurrentWorldPath = "";
	}

	void EditorLayer::SaveWorld()
	{
		if (mCurrentWorldPath.empty())
		{
			SaveWorldAs();
			return;
		}

		AssetFactory::Export(mEditorWorld, mCurrentWorldPath);
	}

	void EditorLayer::SaveWorldAs()
	{
		auto path = FileDialogs::SaveFile(AssetFactory::GetFileFilters());
		if (path.empty())
			return;

		AssetFactory::Export(mEditorWorld, path);
		mCurrentWorldPath = path;
		mAssetManager.ImportAsset(mCurrentWorldPath, AssetType::get<World>(), mEditorWorld->GetHandle());
	}

	void EditorLayer::LoadWorld()
	{
		auto path = FileDialogs::OpenFile(AssetFactory::GetFileFilters());
		if (path.empty())
			return;

		Ref<Asset> asset = mEditorWorld;
		AssetFactory::Import(asset, path);

		mEditorWorld = Cast<World>(asset);
		mActiveWorld = mEditorWorld;
		mSceneHeirarchyPanel->SetContext(mActiveWorld);
		mCurrentWorldPath = path;
	}

} // namespace BHive