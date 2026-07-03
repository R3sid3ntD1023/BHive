#pragma once

#include "Material.h"

namespace BHive
{
	class Material;

	class BHIVE_API MaterialInstance : public IMaterial
	{
	public:

		MaterialInstance(const Ref<Material> &parent);

		template<typename T>
		void Set(const std::string& name, const T& val)
		{
			Set(name, &val, sizeof(T));
		}

		void Set(const std::string &name, const Ref<Texture> &tex, uint32_t mip = 0);

		MaterialSnapshot CreateSnapshot() const override;

	private:
		void Set(const std::string &name, const void *data, size_t size);

	private:
		Ref<Material> mParent;
		std::unordered_map<std::string, TextureSlot> mTextureOverrides;
		std::unordered_map<std::string, std::vector<std::byte>> mUniformOverrides;
	};
}