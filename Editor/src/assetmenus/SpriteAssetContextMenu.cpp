#include "SpriteAssetContextMenu.h"
#include "subsystem/SubSystem.h"
#include "subsystems/WindowSubSystem.h"

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

    REFLECT_ASSET_MENU(SpriteAssetContextMenu, Sprite)
    REFLECT_ASSET_MENU(FlipbookAssetContextMenu, FlipBook)
    REFLECT_ASSET_MENU(SpriteSheetContextMenu, SpriteSheet)
    REFLECT_ASSET_MENU(AudioAssetContextMenu, AudioSource)
    REFLECT_ASSET_MENU(PhysicsMaterialContextMenu, PhysicsMaterial)
    REFLECT_ASSET_MENU(InputContextContextMenu, InputContext)
    REFLECT_ASSET_MENU(MaterialContextMenu, Material)
    REFLECT_ASSET_MENU(SkeletonContextMenu, Skeleton)
    REFLECT_ASSET_MENU(MeshContextMenu, StaticMesh)
    REFLECT_ASSET_MENU(AnimationContextMenu, SkeletalAnimation)
}
