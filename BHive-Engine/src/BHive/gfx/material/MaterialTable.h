#pragma once

#include "asset/Assethandle.h"
#include "core/Core.h"
#include "Material.h"

namespace BHive
{
	struct BHIVE_API MaterialTable
	{
		AssetHandle<Material> Get(uint32_t index = 0) const;

		void Add(AssetHandle<Material> h);

		void Set(uint32_t index, AssetHandle<Material> h);

		const auto &GetAll() const { return mMaterials; }

		void SetAll(const std::vector<AssetHandle<Material>> &materials);

		void SetAll(AssetHandle<Material> material);

		void Reset();

		void Resize(uint64_t size);

		size_t Count() const;

		AssetHandle<Material> operator[](size_t index) const;

		template <typename A>
		void Save(A &ar) const
		{
			ar(mMaterials);
		}

		template <typename A>
		void Load(A &ar)
		{
			ar(mMaterials);
		}

		REFLECTABLE()

	private:
		std::vector<AssetHandle<Material>> mMaterials;
	};

	REFLECT_EXTERN(MaterialTable)
} // namespace BHive