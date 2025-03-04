#pragma once

#include "MeshComponent.h"

BEGIN_NAMESPACE(BHive)

struct AstroidComponent : public MeshComponent
{
	uint32_t Instances = 0;
	uint32_t Seed = 0;
	float Radius = 100.f;
	float Offset = 2.5f;

	AstroidComponent() = default;

	virtual void Update(float dt) override;

	virtual void Save(cereal::JSONOutputArchive &ar) const override;
	virtual void Load(cereal::JSONInputArchive &ar) override;

	REFLECTABLEV(Component)

protected:
	void InitIndirectMesh(const Ref<BaseMesh> &renderable, Ref<IndirectRenderable> &indirect) override;

private:
	void CalculateMatrices();

private:
	std::vector<glm::mat4> mMatrices;
};

END_NAMESPACE