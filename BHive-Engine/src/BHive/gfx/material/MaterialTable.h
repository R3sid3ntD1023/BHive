#pragma once

#include "core/Core.h"
#include "Material.h"
#include "gfx/registries/Handles.h"

namespace BHive
{
	struct BHIVE_API MaterialTable
	{
		MaterialPtr Get(uint32_t index = 0) const;

		void Add(MaterialPtr h);

		void Set(MaterialPtr h, uint32_t index = 0);

		const auto &GetAll() const { return mMaterials; }

		void SetAll(const std::vector<MaterialPtr> &materials);

		void Reset();

		void Resize(uint64_t size);

		size_t Count() const;

		MaterialPtr operator[](size_t index) const;

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
		std::vector<MaterialPtr> mMaterials;
	};

	REFLECT_EXTERN(MaterialTable)
} // namespace BHive