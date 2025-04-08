#include "GameObject.h"
#include "TransformComponent.h"

namespace BHive
{
	TransformComponent::TransformComponent()
	{
		SetTickEnabled(false);
	}

	void TransformComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(Transform);
	}

	void TransformComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(Transform);
	}

	void TransformComponent::Save(cereal::JSONOutputArchive &ar) const
	{
		ar(MAKE_NVP(Transform));
	}

	void TransformComponent::Load(cereal::JSONInputArchive &ar)
	{
		ar(MAKE_NVP(Transform));
	}

	RTTR_REGISTRATION
	{
		BEGIN_REFLECT(FTransform)
			.property(
				"Translation", &FTransform::get_translation,
				rttr::select_overload<void(const glm::vec3 &)>(&FTransform::set_translation))
				REFLECT_PROPERTY("Rotation", get_rotation, set_rotation) REFLECT_PROPERTY("Scale", get_scale, set_scale);
	}

	REFLECT(TransformComponent)
	{
		BEGIN_REFLECT(TransformComponent) REFLECT_PROPERTY(Transform) COMPONENT_IMPL();
	}
} // namespace BHive