#include "LightComponents.h"
#include "scene/SceneRenderer.h"
#include "scene/Actor.h"

namespace BHive
{

    void SpotLightComponent::OnRender(SceneRenderer *renderer)
    {
        auto transform = GetWorldTransform();
        renderer->SubmitLight(mLight, transform);

        LineRenderer::DrawSphere(mLight.mRadius, 16, {}, mLight.mColor, transform);
        LineRenderer::DrawCone(mLight.mRadius, glm::cos(glm::radians(mLight.mOuterCutOff)), 16, 0xFFFF0000, transform);
        LineRenderer::DrawCone(mLight.mRadius, glm::cos(glm::radians(mLight.mInnerCutOff)), 16, 0xFF00FF00, transform);
    }

    void SpotLightComponent::Serialize(StreamWriter& ar) const
    {
        LightComponent::Serialize(ar);
        ar(mLight);
    }

    void SpotLightComponent::Deserialize(StreamReader& ar)
    {
        LightComponent::Deserialize(ar);
        ar(mLight);
    }

    void PointLightComponent::OnRender(SceneRenderer *renderer)
    {
        renderer->SubmitLight(mLight, GetWorldTransform());
        LineRenderer::DrawSphere(mLight.mRadius, 16, {}, mLight.mColor, GetWorldTransform());
    }

    void PointLightComponent::Serialize(StreamWriter& ar) const
    {
        LightComponent::Serialize(ar);
        ar(mLight);
    }

    void PointLightComponent::Deserialize(StreamReader& ar)
    {
        LightComponent::Deserialize(ar);
        ar(mLight);
    }

    void DirectionalLightComponent::OnRender(SceneRenderer *renderer)
    {
        renderer->SubmitLight(mLight, GetWorldTransform());

        auto forward = GetWorldTransform().get_forward();
        LineRenderer::DrawLine({}, -forward, mLight.mColor, GetWorldTransform());
    }

    void DirectionalLightComponent::Serialize(StreamWriter& ar) const
    {
        LightComponent::Serialize(ar);
        ar(mLight);
    }

    void DirectionalLightComponent::Deserialize(StreamReader& ar)
    {
        LightComponent::Deserialize(ar);
        ar(mLight);
    }

    REFLECT(LightComponent)
    {
        BEGIN_REFLECT(LightComponent);
    }

    REFLECT(PointLightComponent)
    {
        BEGIN_REFLECT(PointLightComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true))
        REQUIRED_COMPONENT_FUNCS()
        REFLECT_PROPERTY("Light", mLight);
    }

    REFLECT(SpotLightComponent)
    {
        BEGIN_REFLECT(SpotLightComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true))
        REQUIRED_COMPONENT_FUNCS()
        REFLECT_PROPERTY("Light", mLight);
    }

    REFLECT(DirectionalLightComponent)
    {
        BEGIN_REFLECT(DirectionalLightComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true))
        REQUIRED_COMPONENT_FUNCS()
        REFLECT_PROPERTY("Light", mLight);
    }

} // namespace BHive
