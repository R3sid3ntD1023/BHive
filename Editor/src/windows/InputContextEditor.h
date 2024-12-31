#pragma once

#include "TAssetEditor.h"
#include "input/InputContext.h"

namespace BHive
{

    class InputContextEditor : public TAssetEditor<InputContext>
    {
    protected:
        virtual const char *GetFileDialogFilter() { return "InputContext (*.input)\0*.input\0"; };
    };
}