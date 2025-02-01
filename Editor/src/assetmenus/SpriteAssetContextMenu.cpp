#include "SpriteAssetContextMenu.h"
#include "subsystem/SubSystem.h"
#include "subsystems/WindowSubSystem.h"

#include "windows/editors/AnimationEditor.h"
#include "windows/editors/AnimGraph/AnimGraphEditor.h"
#include "windows/editors/AudioEditor.h"
#include "windows/editors/FlipBookEditor.h"
#include "windows/editors/InputContextEditor.h"
#include "windows/editors/MaterialEditor.h"
#include "windows/editors/MeshEditor.h"
#include "windows/editors/PhysicsMaterialEditor.h"
#include "windows/editors/SkeletonEditor.h"
#include "windows/editors/SpriteEditor.h"
#include "windows/editors/TextureEditor.h"
#include "windows/editors/PrefabEditor.h"

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
		auto editor = editor_system.AddWindow<TEditor>();
		editor->SetContext(handle);
	}

	REFLECT_ASSET_MENU(FAssetContextMenuBase<SpriteEditor>, Sprite)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<FlipBookEditor>, FlipBook)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<SpriteSheetEditor>, SpriteSheet)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<AudioEditor>, AudioSource)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<PhysicsMaterialEditor>, PhysicsMaterial)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<InputContextEditor>, InputContext)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<MaterialEditor>, Material)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<SkeletonEditor>, Skeleton)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<MeshEditor>, IRenderableAsset)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<AnimationEditor>, SkeletalAnimation)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<TextureEditor>, Texture2D)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<AnimGraphEditor>, AnimGraph)
	REFLECT_ASSET_MENU(FAssetContextMenuBase<PrefabEditor>, Prefab)
} // namespace BHive
