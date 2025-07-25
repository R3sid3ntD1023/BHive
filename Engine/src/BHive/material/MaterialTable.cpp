#include "MaterialTable.h"
#include "asset/AssetManager.h"

namespace BHive
{
	void MaterialTable::clear()
	{
		mMaterials.clear();
	}

	void MaterialTable::resize(uint64_t size)
	{
		mMaterials.resize(size);
	}

	size_t MaterialTable::size() const
	{
		return mMaterials.size();
	}

	Ref<Material> MaterialTable::get_material(uint32_t index) const
	{
		if (mMaterials.size() > index)
		{
			return mMaterials.at(index);
		}

		return nullptr;
	}

	void MaterialTable::add_material(const Ref<Material> &material)
	{
		mMaterials.push_back(material);
	}

	void MaterialTable::set_material(const Ref<Material> &material, uint32_t index)
	{
		if (mMaterials.size() > index)
		{
			mMaterials[index] = material;
		}
	}

	void MaterialTable::set_materials(const std::vector<Ref<Material>> &materials)
	{
		for (auto &m : materials)
			mMaterials.emplace_back(m);
	}

	Ref<Material> MaterialTable::operator[](size_t index) const
	{
		ASSERT(index >= 0 && index < mMaterials.size());

		return mMaterials[index];
	}

	REFLECT(MaterialTable)
	{
		BEGIN_REFLECT(MaterialTable)
		REFLECT_PROPERTY("Materials", mMaterials);
	}
} // namespace BHive