#pragma once

#include "TAssetEditor.h"
#include "mesh/Skeleton.h"

namespace BHive
{
    class SkeletonEditor : public TAssetEditor<Skeleton>
    {
    public:
        virtual const char *GetFileDialogFilter() { return "Skeleton (*.skeleton)\0*.skeleton\0"; };
    };

} // namespace BHive
