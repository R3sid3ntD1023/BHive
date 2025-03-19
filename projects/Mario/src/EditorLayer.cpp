#include "EditorLayer.h"
#include "GUI/Gui.h"
#include "gfx/Framebuffer.h"
#include "core/Application.h"
#include "core/Window.h"
#include "gfx/RenderCommand.h"
#include "renderers/Renderer.h"
#include "editor/WindowSubSystem.h"
#include "subsystem/SubSystem.h"
#include "Inspectors.h"

namespace BHive
{
	void EditorLayer::OnAttach()
	{
		SetCurrentContext(ImGui::GetCurrentContext());

		auto &window = Application::Get().GetWindow();
		auto &size = window.GetSize();

		FramebufferSpecification specs{};
		specs.Width = size.x;
		specs.Height = size.y;
		specs.Attachments.attach({.InternalFormat = EFormat::RGBA8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});
		mFramebuffer = CreateRef<Framebuffer>(specs);

		mEditorWorld = CreateRef<World>("EditorWorld");

		RenderCommand::ClearColor(.2f, .2f, .2f);

		float aspect = size.x / (float)size.y;
		mEditorCamera = EditorCamera(75.f, aspect, .01f, 100.f);

		auto &window_system = SubSystemContext::Get().AddSubSystem<WindowSubSystem>();
		mSceneHeirarchyPanel = window_system.CreateWindow<SceneHierarchyPanel>();
		mSceneHeirarchyPanel->SetContext(mEditorWorld);
		mPropertiesPanel = window_system.CreateWindow<PropertiesPanel>();
		mPropertiesPanel->GetSelectedObjectEvent.bind(mSceneHeirarchyPanel.get(), &SceneHierarchyPanel::GetSelectedObject);

		mContentBrowser = window_system.CreateWindow<EditorContentBrowser<EditorAssetManager>>(RESOURCE_PATH);

		FProjectConfiguration config{};
		config.mName = "Mario";
		config.mProjectDirectory = RESOURCE_PATH "../";
		config.mResourcesDirectory = "resources";
		Project::New(config);
		AssetManager::SetAssetManager(&mAssetManager);
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

		mEditorCamera.ProcessInput();

		Renderer::SubmitCamera(mEditorCamera.GetProjection(), mEditorCamera.GetView());

		mFramebuffer->Bind();

		RenderCommand::Clear();

		Renderer::Begin();

		mEditorWorld->Update(dt);

		LineRenderer::DrawGrid(FGrid{.color = 0x808080ff, .stepcolor = 0x808080ff});

		Renderer::End();

		mFramebuffer->UnBind();
	}

	void EditorLayer::OnEvent(Event &e)
	{
		mEditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch(this, &EditorLayer::OnWindowResize);
	}

	void EditorLayer::OnGuiRender()
	{
		static bool viewport_status = true;
		static bool scene_heirarchy_status = true;
		static bool properties_status = true;
		static bool content_browser_status = true;

		GUI::BeginDockSpace("DockSpace");

		SubSystemContext::Get().GetSubSystem<WindowSubSystem>().UpdateWindows();

		if (ImGui::Begin("Viewport", &viewport_status))
		{
			auto size = ImGui::GetContentRegionAvail();
			mViewportSize = {size.x, size.y};

			ImGui::Image((ImTextureID)(uint64_t)*mFramebuffer->GetColorAttachment(), size, {0, 1}, {1, 0});
		}

		ImGui::End();

		GUI::EndDockSpace();
	}

	bool EditorLayer::OnWindowResize(WindowResizeEvent &e)
	{

		return false;
	}

} // namespace BHive