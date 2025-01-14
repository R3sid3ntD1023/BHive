#pragma once

#include "core/Core.h"
#include "Material.h"

namespace BHive
{
	class Material;

	struct BHIVE MaterialTable
	{

		using material_list = std::vector<TAssetHandle<Material>>;

		void clear();

		void resize(uint64_t size);

		size_t size() const;

		TAssetHandle<Material> get_material(uint32_t index = 0) const;

		void add_material(const TAssetHandle<Material> &material);

		void set_material(const TAssetHandle<Material> &material, uint32_t index = 0);

		const material_list &get_materials() const { return mMaterials; }

		material_list &get_materials() { return mMaterials; }

		void set_materials(const std::vector<Ref<Material>> &materials);

		TAssetHandle<Material> operator[](size_t index) const;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(MAKE_NVP("Materials", mMaterials));
		}

	private:
		material_list mMaterials;
		REFLECTABLE()
	};

	REFLECT_EXTERN(MaterialTable);

} // namespace BHive