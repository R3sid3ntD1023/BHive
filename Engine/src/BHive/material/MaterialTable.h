#pragma once

#include "core/Core.h"
#include "Material.h"
#include "serialization/Serialization.h"

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

		void Serialize(StreamWriter &ar) const;
		void Deserialize(StreamReader &ar);

	private:
		material_list mMaterials;
		REFLECTABLE()
	};

	REFLECT_EXTERN(MaterialTable);

}