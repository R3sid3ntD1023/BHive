#include "inspector/Inspector.h"

namespace BHive
{
    void Inspector::BeginInspect(const rttr::property &property)
    {
        mLayout = PropertyLayout(property, true);
        mLayout.PushLayout();
    }

    void Inspector::EndInspect(const rttr::property &property)
    {
        mLayout.PopLayout();
    }
}