#pragma once

#include "StaticMeshComponent.h"

namespace BHive
{
	struct BHIVE_API InstancedStaticMeshComponent : public StaticMeshComponent
	{

		void SetInstances(const std::vector<FTransform> &transforms);

		int32_t GetCount() { return (uint32_t)mInstances.size(); }

		const std::vector<glm::mat4> &GetInstances() const { return mInstances; }

		void Save(cereal::BinaryOutputArchive &ar) const override;

		void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(StaticMeshComponent)

	protected:
		std::vector<glm::mat4> mInstances;
	};

	REFLECT_EXTERN(InstancedStaticMeshComponent)
} // namespace BHive