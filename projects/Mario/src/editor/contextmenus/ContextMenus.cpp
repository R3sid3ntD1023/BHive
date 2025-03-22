#include "ContextMenus.h"
#include "core/subsystem/SubSystem.h"
#include "editor/subsystems/WindowSubSystem.h"

#include "editor/editorwindows/AudioEditor.h"
#include "editor/editorwindows/FlipBookEditor.h"
#include "editor/editorwindows/InputContextEditor.h"
#include "editor/editorwindows/SpriteEditor.h"
#include "editor/editorwindows/TextureEditor.h"
#include "editor/editorwindows/SpriteSheetEditor.h"
#include "physics/PhysicsMaterial.h"
#include "objects/World.h"

namespace BHive
{
	template <typename TEditor>
	void FAssetContextMenuBase<TEditor>::OnAssetContext(const UUID &handle)
	{
		if (ImGui::MenuItem("Edit"))
		{
			OnAssetOpen(handle);
		}
	}

	template <typename TEditor>
	void FAssetContextMenuBase<TEditor>::OnAssetOpen(const UUID &handle)
	{
		auto &editor_system = SubSystemContext::Get().GetSubSystem<WindowSubSystem>();
		auto editor = editor_system.CreateWindow<TEditor>();
		editor->SetContext(handle);
	}

	void FWorldContentMenu::OnAssetContext(const UUID &handle)
	{
		if (ImGui::MenuItem("Open"))
		{
			OnAssetOpen(handle);
		}
	}

	void FWorldContentMenu::OnAssetOpen(const UUID &handle)
	{
		if (OnAssetOpenedEvent)
		{
			OnAssetOpenedEvent.invoke(handle);
		}
	}

	REFLECT_ASSET_MENU(FAssetContextMenuBase<SpriteEditor>, Sprite)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<FlipBookEditor>, FlipBook)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<SpriteSheetEditor>, SpriteSheet)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<AudioEditor>, AudioSource)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<TAssetEditor<PhysicsMaterial>>, PhysicsMaterial)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<InputContextEditor>, InputContext)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<TextureEditor>, Texture2D)
	REFLECT_ASSET_MENU(FWorldContentMenu, World);

} // namespace BHive
