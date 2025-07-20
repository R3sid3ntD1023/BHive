#include "ContextMenus.h"
#include "core/subsystem/SubSystem.h"
#include "subsystems/WindowSubSystem.h"

#include "editorwindows/AudioEditor.h"
#include "editorwindows/FlipBookEditor.h"
#include "editorwindows/InputContextEditor.h"
#include "editorwindows/SpriteEditor.h"
#include "editorwindows/TextureEditor.h"
#include "editorwindows/SpriteSheetEditor.h"
#include "physics/PhysicsMaterial.h"
#include "editorwindows/MeshEditor.h"
#include "editorwindows/MaterialEditor.h"
#include "editorwindows/AnimationEditor.h"
#include "editorwindows/SkeletonEditor.h"
#include "editorwindows/AnimGraph/AnimGraphEditor.h"

#include "mesh/StaticMesh.h"
#include "mesh/SkeletalMesh.h"
#include "mesh/SkeletalAnimation.h"
#include "world/World.h"

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
	REFLECT_ASSET_MENU(FAssetContextMenuBase<MeshEditor>, StaticMesh, SkeletalMesh);
	REFLECT_ASSET_MENU(FAssetContextMenuBase<MaterialEditor>, Material);
	REFLECT_ASSET_MENU(FAssetContextMenuBase<AnimationEditor>, SkeletalAnimation);
	REFLECT_ASSET_MENU(FAssetContextMenuBase<SkeletonEditor>, Skeleton);
	REFLECT_ASSET_MENU(FAssetContextMenuBase<AnimGraphEditor>, AnimGraph);

} // namespace BHive
