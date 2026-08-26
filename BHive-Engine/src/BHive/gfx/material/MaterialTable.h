#pragma once

#include "core/Core.h"
#include "Material.h"
#include "gfx/registries/ResourceHandle.h"

namespace BHive
{
	struct BHIVE_API MaterialTable
	{
		ResourceHandle Get(uint32_t index = 0) const;

		void Add(ResourceHandle h);

		void Set(ResourceHandle h, uint32_t index = 0);

		const auto &GetAll() const { return mMaterials; }

		void SetAll(const std::vector<ResourceHandle> &materials);

		void Reset();

		void Resize(uint64_t size);

		size_t Count() const;

		ResourceHandle operator[](size_t index) const;

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
		std::vector<ResourceHandle> mMaterials;
	};

	REFLECT_EXTERN(MaterialTable)
} // namespace BHive