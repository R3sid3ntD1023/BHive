#pragma once

#include "Material.h"

namespace BHive
{
	class Material;

	class BHIVE_API MaterialInstance : public IMaterial
	{
	public:
		MaterialInstance(Ref<Material> parent);

		IMaterial &SetParam(const std::string &name, const MaterialParam &val) & override;

		IMaterial &SetTexture(const std::string &name, const TextureBinding &texture) & override;

		MaterialSnapshot CreateSnapshot() const override;
		MaterialSnapshot CreateSnapshot(EShaderMeshType meshType) const override;

	private:
		Ref<Material> mParent;
		std::unordered_map<uint64_t, TextureBinding> mTextureOverrides;
		std::unordered_map<uint64_t, MaterialParam> mUniformOverrides;
	};
} // namespace BHive