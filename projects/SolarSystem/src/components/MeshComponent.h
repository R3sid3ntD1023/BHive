#pragma once

#include <core/EnumAsByte.h>
#include "Component.h"

BEGIN_NAMESPACE(BHive)

class IndirectRenderable;
class ShaderInstance;
class BaseMesh;

enum EFlags : uint32_t
{
	EFlags_None = 0,
	EFlags_Single_Channel_Texture = 1 << 0
};

struct MeshComponent : public Component
{
	glm::vec3 Color{1.0f};
	glm::vec3 Emission{0.0f};
	TEnumAsByte<EFlags> Flags = EFlags::EFlags_None;
	UUID Texture;
	UUID Mesh;

	MeshComponent();

	virtual void Update(float dt) override;
	virtual void Save(cereal::JSONOutputArchive &ar) const override;
	virtual void Load(cereal::JSONInputArchive &ar) override;

	REFLECTABLEV(Component)

protected:
	virtual void InitIndirectMesh(const Ref<BaseMesh> &renderable, Ref<IndirectRenderable> &indirect);

protected:
	Ref<IndirectRenderable> mIndirectMesh;
	Ref<ShaderInstance> mShaderInstance;
};

END_NAMESPACE