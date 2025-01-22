#pragma once

#include "asset/FAssetContextMenu.h"
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

namespace BHive
{
	class AssetEditor;

	template <typename TEditor>
	struct FAssetContextMenuBase : public FAssetContextMenu
	{
		virtual void OnAssetContext(const AssetHandle &handle) override;
		virtual void OnAssetOpen(const AssetHandle &handle) override;
	};

	struct SpriteAssetContextMenu : public FAssetContextMenuBase<SpriteEditor>
	{
		REFLECTABLEV(FAssetContextMenu)
	};

	struct FlipbookAssetContextMenu : public FAssetContextMenuBase<FlipBookEditor>
	{
		REFLECTABLEV(FAssetContextMenu)
	};

	struct AudioAssetContextMenu : public FAssetContextMenuBase<AudioEditor>
	{
		REFLECTABLEV(FAssetContextMenu)
	};

	struct PhysicsMaterialContextMenu : public FAssetContextMenuBase<PhysicsMaterialEditor>
	{
		REFLECTABLEV(FAssetContextMenu)
	};

	struct SpriteSheetContextMenu : public FAssetContextMenuBase<SpriteSheetEditor>
	{
		REFLECTABLEV(FAssetContextMenu)
	};

	struct InputContextContextMenu : public FAssetContextMenuBase<InputContextEditor>
	{
		REFLECTABLEV(FAssetContextMenu)
	};

	struct MaterialContextMenu : public FAssetContextMenuBase<MaterialEditor>
	{
		REFLECTABLEV(FAssetContextMenu)
	};

	struct SkeletonContextMenu : public FAssetContextMenuBase<SkeletonEditor>
	{
		REFLECTABLEV(FAssetContextMenu)
	};

	struct MeshContextMenu : public FAssetContextMenuBase<MeshEditor>
	{
		REFLECTABLEV(FAssetContextMenu)
	};

	struct AnimationContextMenu : public FAssetContextMenuBase<AnimationEditor>
	{
		REFLECTABLEV(FAssetContextMenu)
	};

	struct TextureContextMenu : public FAssetContextMenuBase<TextureEditor>
	{
		REFLECTABLEV(FAssetContextMenu)
	};

	struct AnimGraphContextMenu : public FAssetContextMenuBase<AnimGraphEditor>
	{
		REFLECTABLEV(FAssetContextMenu)
	};

} // namespace BHive
