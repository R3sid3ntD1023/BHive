#pragma once

#include "core/Core.h"
#include "Material.h"

namespace BHive
{
	class Material;

	struct BHIVE MaterialTable
	{

		using material_list = std::vector<Ref<Material>>;

		void clear();

		void resize(uint64_t size);

		size_t size() const;

		Ref<Material> get_material(uint32_t index = 0) const;

		void add_material(const Ref<Material> &material);

		void set_material(const Ref<Material> &material, uint32_t index = 0);

		const material_list &get_materials() const { return mMaterials; }

		material_list &get_materials() { return mMaterials; }

		void set_materials(const std::vector<Ref<Material>> &materials);

		Ref<Material> operator[](size_t index) const;

		template <typename A>
		void Serialize(A &ar)
		{
			// ar(mMaterials);
		}

	private:
		material_list mMaterials;
	};
} // namespace BHive