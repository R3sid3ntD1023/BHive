#include "asset/AssetManager.h"
#include "MaterialTable.h"

namespace BHive
{
	void MaterialTable::Reset()
	{
		mMaterials.clear();
	}

	void MaterialTable::Resize(uint64_t size)
	{
		mMaterials.resize(size);
	}

	size_t MaterialTable::Count() const
	{
		return mMaterials.size();
	}

	AssetHandle<Material> MaterialTable::Get(uint32_t index) const
	{
		if (mMaterials.size() > index)
		{
			return mMaterials.at(index);
		}

		return {};
	}

	void MaterialTable::Add(AssetHandle<Material> h)
	{
		mMaterials.emplace_back(h);
	}

	void MaterialTable::Set(uint32_t index, AssetHandle<Material> h)
	{
		if (mMaterials.size() > index)
		{
			mMaterials[index] = h;
		}
	}

	void MaterialTable::SetAll(const std::vector<AssetHandle<Material>> &materials)
	{
		mMaterials = materials;
	}

	void MaterialTable::SetAll(AssetHandle<Material> material)
	{
		for (size_t i = 0; i < mMaterials.size(); i++)
		{
			mMaterials[i] = material;
		}
	}

	AssetHandle<Material> MaterialTable::operator[](size_t index) const
	{
		ASSERT(index < mMaterials.size());

		return mMaterials[index];
	}

	REFLECT(MaterialTable)
	{
		BEGIN_REFLECT(MaterialTable) REFLECT_PROPERTY("Materials", mMaterials);
	}
} // namespace BHive