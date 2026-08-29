#include "MaterialTable.h"
#include "asset/AssetManager.h"

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

	ResourceHandle MaterialTable::Get(uint32_t index) const
	{
		if (mMaterials.size() > index)
		{
			return mMaterials.at(index);
		}

		return {};
	}

	void MaterialTable::Add(ResourceHandle h)
	{
		mMaterials.emplace_back(h);
	}

	void MaterialTable::Set(ResourceHandle h, uint32_t index)
	{
		if (mMaterials.size() > index)
		{
			mMaterials[index] = h;
		}
	}

	void MaterialTable::SetAll(const std::vector<ResourceHandle> &materials)
	{
		mMaterials = materials;
	}

	ResourceHandle MaterialTable::operator[](size_t index) const
	{
		ASSERT(index < mMaterials.size());

		return mMaterials[index];
	}

	REFLECT(MaterialTable)
	{
		BEGIN_REFLECT(MaterialTable)
		REFLECT_PROPERTY("Materials", mMaterials);
	}
} // namespace BHive