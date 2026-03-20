#pragma once

#include "core/Core.h"
#include "Material.h"

namespace BHive
{
	class Material;

	struct BHIVE_API MaterialTable
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
		void Save(A &ar) const
		{
			ar(cereal::make_size_tag(mMaterials.size()));
			for (auto &material : mMaterials)
			{
				ar(TAssetHandle(material));
			}
		}

		template <typename A>
		void Load(A &ar)
		{
			size_t size = 0;
			ar(cereal::make_size_tag(size));
			mMaterials.resize(size);
			for (size_t i = 0; i < size; i++)
			{
				ar(TAssetHandle(mMaterials[i]));
			}
		}

		REFLECTABLE()

	private:
		material_list mMaterials;
	};

	REFLECT_EXTERN(MaterialTable)
} // namespace BHive