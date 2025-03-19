#include "ContextMenus.h"
#include "core/subsystem/SubSystem.h"
#include "editor/WindowSubSystem.h"

#include "editor/editorwindows/AudioEditor.h"
#include "editor/editorwindows/FlipBookEditor.h"
#include "editor/editorwindows/InputContextEditor.h"
#include "editor/editorwindows/SpriteEditor.h"
#include "editor/editorwindows/TextureEditor.h"
#include "editor/editorwindows/SpriteSheetEditor.h"
#include "physics/PhysicsMaterial.h"

namespace BHive
{
	template <typename TEditor>
	void FAssetContextMenuBase<TEditor>::OnAssetContext(const AssetHandle &handle)
	{
		if (ImGui::MenuItem("Edit"))
		{
			OnAssetOpen(handle);
		}
	}

	template <typename TEditor>
	void FAssetContextMenuBase<TEditor>::OnAssetOpen(const AssetHandle &handle)
	{
		auto &editor_system = SubSystemContext::Get().GetSubSystem<WindowSubSystem>();
		auto editor = editor_system.CreateWindow<TEditor>();
		editor->SetContext(handle);
	}

	REFLECT_ASSET_MENU(FAssetContextMenuBase<SpriteEditor>, Sprite)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<FlipBookEditor>, FlipBook)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<SpriteSheetEditor>, SpriteSheet)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<AudioEditor>, AudioSource)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<TAssetEditor<PhysicsMaterial>>, PhysicsMaterial)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<InputContextEditor>, InputContext)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<TextureEditor>, Texture2D)
} // namespace BHive
