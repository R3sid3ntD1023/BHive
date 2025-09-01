#pragma once

#include "asset/EditorAssetManager.h"
#include "core/Command.h"
#include "core/events/ApplicationEvents.h"
#include "core/events/KeyEvents.h"
#include "core/Layer.h"
#include "EditorContentBrowser.h"
#include "gfx/cameras/EditorCamera.h"
#include "renderers/LineRenderer.h"

namespace BHive
{
	class PickerRenderPass;
	class OutlineRenderPass;
	class OutlinePostProcessRenderPass;
	class SceneRenderer;
	class ImSceneHierarchy;
	class World;

	struct FGizmoData
	{
		int32_t Operation = 0;
		int32_t Mode = 0;
		int32_t IsSnappingEnabled{0};
		std::unordered_map<uint8_t, glm::vec3> SnapValues;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(MAKE_NVP(Operation), MAKE_NVP(Mode), MAKE_NVP(IsSnappingEnabled), MAKE_NVP(SnapValues));
		}
	};

	struct FWindowLayout
	{
		std::string Name;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(MAKE_NVP(Name));
		}
	};

	struct EditorStyles
	{
		FGrid GridStyle{.size = 40.f, .divisions = 20, .color = 0xffffffff, .stepcolor = 0xff808080};
	};

	class EditorLayer : public Layer
	{
	private:
		Ref<SceneRenderer> mRenderer;
		Ref<PickerRenderPass> mPickerRenderPass;
		Ref<OutlineRenderPass> mOutlineRenderPass;
		Ref<OutlinePostProcessRenderPass> mOutlinePostProcessPass;

		Ref<World> mEditorWorld;
		Ref<World> mActiveWorld;

		Ref<ImSceneHierarchy> mSceneHeirarchyPanel;
		Ref<EditorContentBrowser<EditorAssetManager>> mContentBrowser;
		EditorCamera mEditorCamera;

		Ref<EditorAssetManager> mAssetManager;

		glm::ivec2 mViewportSize{};
		glm::ivec2 mViewportBounds[2] = {};
		bool mViewportHovered{false}, mViewportFocused{false};

		std::unordered_map<FCommand, std::function<void()>> mCommands;

		std::filesystem::path mCurrentWorldPath = "";

		FGizmoData mGizmo;
		FWindowLayout mCurrentLayout{"default.ini"};
		EditorStyles mStyles;

	public:
		void OnAttach() override;
		void OnUpdate(float) override;
		void OnDetach() override;
		void OnEvent(Event &e) override;
		void OnGuiRender() override;
		void SetupDefaultCommands();

		template <typename A>
		void Serialize(A &ar)
		{
			ar(MAKE_NVP("Layout", mCurrentLayout), MAKE_NVP("Camera", mEditorCamera), MAKE_NVP("Gizmo", mGizmo));
		}

	private:
		void InitRenderer(const glm::uvec2 &size);

		// add editor-only render passes
		void InitRenderPasses();

		bool OnWindowResize(WindowResizeEvent &e);
		bool OnKeyEvent(KeyEvent &e);

		void CreateWorld();
		void SaveWorld();
		void SaveWorldAs();
		void LoadWorld();
		void LoadWorld(const std::filesystem::path &path);
		void SetActiveWorld(const Ref<World> &world);
		bool LoadProjectLibrary(const std::string &lib);

		void OpenProject(const std::filesystem::path &path);
		void OnProjectOpened();
		void LoadEditorConfigFile();
		void SaveEditorConfigFile();

#pragma region GUI
		void ViewportGUI();
		void Viewport();

#pragma endregion
	};

} // namespace BHive
