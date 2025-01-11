#pragma once

#include "asset/FAssetContextMenu.h"
#include "windows/SpriteEditor.h"
#include "windows/FlipBookEditor.h"
#include "windows/AudioEditor.h"
#include "windows/PhysicsMaterialEditor.h"
#include "windows/InputContextEditor.h"
#include "windows/MaterialEditor.h"
#include "windows/SkeletonEditor.h"
#include "windows/MeshEditor.h"
#include "windows/AnimationEditor.h"
#include "windows/TextureEditor.h"

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

} // namespace BHive
