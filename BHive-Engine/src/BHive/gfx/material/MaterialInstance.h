#pragma once

#include "Material.h"

namespace BHive
{
	class Material;

	class BHIVE_API MaterialInstance : public IMaterial
	{
	public:
		MaterialInstance(const Ref<Material> &parent);

		IMaterial &SetParam(const std::string &name, const MaterialParam &val) & override;

		IMaterial &SetTexture(const std::string &name, const FTextureBinding &texture) & override;

		MaterialSnapshot CreateSnapshot() const override;

	private:
		Ref<Material> mParent;
		std::unordered_map<std::string, FTextureBinding> mTextureOverrides;
		std::unordered_map<std::string, MaterialParam> mUniformOverrides;
	};
} // namespace BHive