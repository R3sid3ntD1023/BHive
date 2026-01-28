#include "LightComponents.h"
#include "world/GameObject.h"

namespace BHive
{

	void DirectionalLightComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(MAKE_NVP(Color));
	}
	void DirectionalLightComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(MAKE_NVP(Color));
	}

	void PointLightComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(MAKE_NVP(Color), MAKE_NVP(Radius));
	}

	void PointLightComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(MAKE_NVP(Color), MAKE_NVP(Radius));
	}

	void SpotLightComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(MAKE_NVP(Color), MAKE_NVP(Radius), MAKE_NVP(InnerCutoff), MAKE_NVP(OuterCutoff));
	}

	void SpotLightComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(MAKE_NVP(Color), MAKE_NVP(Radius), MAKE_NVP(InnerCutoff), MAKE_NVP(OuterCutoff));
	}

	REFLECT(DirectionalLightComponent)
	{
		BEGIN_REFLECT(DirectionalLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Color)(META_DATA(EPropertyMetaData_HDR, true)) COMPONENT_IMPL();
	}

	REFLECT(PointLightComponent)
	{
		BEGIN_REFLECT(PointLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Color)(META_DATA(EPropertyMetaData_HDR, true))
			REFLECT_PROPERTY(Radius)(META_DATA(EPropertyMetaData_Min, 0.0f)) COMPONENT_IMPL();
	}

	REFLECT(SpotLightComponent)
	{
		BEGIN_REFLECT(SpotLightComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Color)(META_DATA(EPropertyMetaData_HDR, true))
			REFLECT_PROPERTY(Radius)(META_DATA(EPropertyMetaData_Min, 0.0f)) REFLECT_PROPERTY(InnerCutoff)(META_DATA(EPropertyMetaData_Min, 0.0f))
				REFLECT_PROPERTY(OuterCutoff)(META_DATA(EPropertyMetaData_Min, 0.0f)) COMPONENT_IMPL();
	}

} // namespace BHive