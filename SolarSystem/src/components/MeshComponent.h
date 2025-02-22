#pragma once

#include <core/EnumAsByte.h>
#include "Component.h"

namespace BHive
{
	class IndirectRenderable;
	class ShaderInstance;
	class BindlessTexture;
	class IRenderableAsset;
} // namespace BHive

enum EFlags : uint32_t
{
	EFlags_None = 0,
	EFlags_Single_Channel_Texture = 1 << 0
};

struct MeshComponent : public Component
{
	glm::vec3 Color{1.0f};
	glm::vec3 Emission{0.0f};
	BHive::TEnumAsByte<EFlags> Flags = EFlags::EFlags_None;
	BHive::UUID Texture;
	BHive::UUID Mesh;

	MeshComponent();

	virtual void Update(float dt) override;
	virtual void Save(cereal::JSONOutputArchive &ar) const override;
	virtual void Load(cereal::JSONInputArchive &ar) override;

	REFLECTABLEV(Component)

protected:
	virtual void InitIndirectMesh(const Ref<BHive::IRenderableAsset> &renderable, Ref<BHive::IndirectRenderable> &indirect);

protected:
	Ref<BHive::IndirectRenderable> mIndirectMesh;
	Ref<BHive::ShaderInstance> mShaderInstance;
	Ref<BHive::BindlessTexture> mBindlessTexture;
};
