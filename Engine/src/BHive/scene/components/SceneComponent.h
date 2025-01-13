#pragma once

#include "scene/Component.h"
#include "scene/ITransform.h"
#include "RelationshipComponent.h"

namespace BHive
{
	class SceneComponent : public Component, public ITransform
	{
	public:
		void SetWorldTransform(const FTransform &transform);

		FTransform GetWorldTransform() const;

		void SetLocalTransform(const FTransform &transform);

		const FTransform &GetLocalTransform() const;

		virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

	private:
		FTransform mLocalTransform;

		REFLECTABLEV(Component, ITransform)
	};

	REFLECT_EXTERN(SceneComponent)
} // namespace BHive
