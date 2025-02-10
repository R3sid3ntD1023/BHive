#pragma once

#include <core/EnumAsByte.h>
#include "Component.h"

enum EFlags : uint32_t
{
	EFlags_None = 0,
	EFlags_Single_Channel_Texture = 1 << 0
};

struct MeshComponent : public Component
{
	glm::vec3 mColor{1.0f};
	glm::vec3 mEmission{0.0f};
	BHive::TEnumAsByte<EFlags> mFlags = EFlags::EFlags_None;
	BHive::UUID mTextureHandle;
	BHive::UUID mMeshHandle;

	virtual void Save(cereal::JSONOutputArchive &ar) const override;
	virtual void Load(cereal::JSONInputArchive &ar) override;

	REFLECTABLEV(Component)
};
