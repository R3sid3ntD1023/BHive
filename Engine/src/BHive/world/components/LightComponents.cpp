#include "LightComponents.h"
#include "world/GameObject.h"

namespace BHive
{

	void DirectionalLightComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(MAKE_NVP(Light));
	}
	void DirectionalLightComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(MAKE_NVP(Light));
	}

	void PointLightComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(MAKE_NVP(Light));
	}

	void PointLightComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(MAKE_NVP(Light));
	}

	void SpotLightComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(MAKE_NVP(Light));
	}

	void SpotLightComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(MAKE_NVP(Light));
	}

	REFLECT(LightBase)
	{
		BEGIN_REFLECT(LightBase)
		REFLECT_PROPERTY("Color", Color)(META_DATA(EPropertyMetaData_HDR, true));
	}

	REFLECT(PointLight)
	{
		BEGIN_REFLECT(PointLight)
		REFLECT_PROPERTY("Radius", Radius)(META_DATA(EPropertyMetaData_Min, 0.0f));
	}

	REFLECT(SpotLight)
	{
		BEGIN_REFLECT(SpotLight)
		REFLECT_PROPERTY("InnerCutOff", InnerCutOff)(META_DATA(EPropertyMetaData_Min, 0.0f)) REFLECT_PROPERTY("OuterCutOff", OuterCutOff)(META_DATA(EPropertyMetaData_Min, 0.0f));
	}

	REFLECT(DirectionalLight)
	{
		BEGIN_REFLECT(DirectionalLight);
	}

	REFLECT(DirectionalLightComponent)
	{
		BEGIN_REFLECT(DirectionalLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Light", Light) COMPONENT_IMPL();
	}

	REFLECT(PointLightComponent)
	{
		BEGIN_REFLECT(PointLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Light", Light) COMPONENT_IMPL();
	}

	REFLECT(SpotLightComponent)
	{
		BEGIN_REFLECT(SpotLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Light", Light) COMPONENT_IMPL();
	}

} // namespace BHive