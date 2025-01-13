#include "Lights.h"

namespace BHive
{
    REFLECT(Light)
    {
        BEGIN_REFLECT(Light)
        REFLECT_PROPERTY("Color", mColor)
        REFLECT_PROPERTY("Brightness", mBrightness)
        (META_DATA(EPropertyMetaData_Min, 0.0f));
    }

    REFLECT(PointLight)
    {
        BEGIN_REFLECT(PointLight)
        REFLECT_PROPERTY("Radius", mRadius);
    }

    REFLECT(SpotLight)
    {
        BEGIN_REFLECT(SpotLight)
        REFLECT_PROPERTY("OuterCutOff", mOuterCutOff)
        (META_DATA(EPropertyMetaData_Min, 0.0f))
            REFLECT_PROPERTY("InnerCutOff", mInnerCutOff)(META_DATA(EPropertyMetaData_Min, 0.0f));
    }

    REFLECT(DirectionalLight)
    {
        BEGIN_REFLECT(DirectionalLight);
    }

} // namespace BHive
