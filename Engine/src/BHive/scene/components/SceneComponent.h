#pragma once

#include "RelationshipComponent.h"
#include "scene/Component.h"
#include "scene/ITransform.h"

namespace BHive
{
	class SceneComponent : public Component, public ITransform
	{
	public:
		void SetWorldTransform(const FTransform &transform);

		FTransform GetWorldTransform() const;

		void SetLocalTransform(const FTransform &transform);

		const FTransform &GetLocalTransform() const;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

	private:
		FTransform mLocalTransform;

		REFLECTABLEV(Component, ITransform)
	};

	REFLECT_EXTERN(SceneComponent)
} // namespace BHive
