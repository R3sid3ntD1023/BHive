#pragma once

#include "TAssetEditor.h"
#include "physics/PhysicsMaterial.h"

namespace BHive
{
    class PhysicsMaterialEditor : public TAssetEditor<PhysicsMaterial>
    {

    protected:
        virtual const char *GetFileDialogFilter() { return "PhysicsMaterial (*.physicsmaterial)\0*.physicsmaterial\0"; };
    };
}