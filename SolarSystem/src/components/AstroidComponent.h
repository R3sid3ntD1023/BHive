#pragma once

#include "MeshComponent.h"

struct AstroidComponent : public Component
{
	uint32_t mNumInstances = 0;
	uint32_t mSeed = 0;
	float mRadius = 100.f;
	float mOffset = 2.5f;
	std::vector<BHive::UUID> mMeshHandles;

	glm::vec3 mColor{1.0f};
	glm::vec3 mEmission{0.0f};
	BHive::TEnumAsByte<EFlags> mFlags = EFlags::EFlags_None;
	BHive::UUID mTextureHandle;

	AstroidComponent() = default;

	const std::vector<glm::mat4> &GetMatrices() const { return mMatrices; }

	virtual void Save(cereal::JSONOutputArchive &ar) const override;
	virtual void Load(cereal::JSONInputArchive &ar) override;

	REFLECTABLEV(Component)

private:
	void CalculateMatrices();

private:
	std::vector<glm::mat4> mMatrices;
};