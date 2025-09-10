#pragma once

#include "StaticMeshComponent.h"

namespace BHive
{
	struct BHIVE_API InstancedStaticMeshComponent : public StaticMeshComponent
	{

		void SetInstances(const std::vector<FTransform> &transforms);

		int32_t GetCount() { return mInstances.size(); }

		const std::vector<glm::mat4> &GetInstances() const { return mInstances; }

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(StaticMeshComponent)

	protected:
		std::vector<glm::mat4> mInstances;
	};

	REFLECT_EXTERN(InstancedStaticMeshComponent)
} // namespace BHive