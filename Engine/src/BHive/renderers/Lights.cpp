#include "Lights.h"

namespace BHive
{
    void Light::Serialize(StreamWriter &ar) const
    {
        ar(mColor, mBrightness);
    }

    void Light::Deserialize(StreamReader &ar)
    {
        ar(mColor, mBrightness);
    }

    void SpotLight::Serialize(StreamWriter &ar) const
    {
        Light::Serialize(ar);
        ar(mRadius, mInnerCutOff, mOuterCutOff);
    }

    void SpotLight::Deserialize(StreamReader &ar)
    {
        Light::Deserialize(ar);
        ar(mRadius, mInnerCutOff, mOuterCutOff);
    }

    void PointLight::Serialize(StreamWriter &ar) const
    {
        Light::Serialize(ar);
        ar(mRadius);
    }

    void PointLight::Deserialize(StreamReader &ar)
    {
        Light::Deserialize(ar);
        ar(mRadius);
    }

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
        REFLECT_PROPERTY("Radius", mRadius)
        REFLECT_PROPERTY("OuterCutOff", mOuterCutOff)
        (META_DATA(EPropertyMetaData_Min, 0.0f))
            REFLECT_PROPERTY("InnerCutOff", mInnerCutOff)(META_DATA(EPropertyMetaData_Min, 0.0f));
    }

    REFLECT(DirectionalLight)
    {
        BEGIN_REFLECT(DirectionalLight);
    }

} // namespace BHive
