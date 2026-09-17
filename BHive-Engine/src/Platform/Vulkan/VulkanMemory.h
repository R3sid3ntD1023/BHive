#pragma once

#include "ImageState.h"
#include "MemoryAllocator.h"
#include "gfx/NativeHandle.h"
#include "gfx/ResourceID.h"

namespace BHive
{
	struct ViewKey
	{
		uint32_t Layer = 0, Mip = 0;

		bool operator<(const ViewKey &rhs) const
		{
			if (Layer != rhs.Layer)
				return Layer < rhs.Layer;

			return Mip < rhs.Mip;
		}
	};

	struct ImageViews
	{
		ResourceID Default;

		std::map<ViewKey, ResourceID> Views;

		auto begin() { return Views.begin(); }

		auto end() { return Views.end(); }
	};

	struct ImageStateTracker
	{
		std::vector<std::vector<ImageState>> MipStates;

		std::map<ViewKey, ImageState> States;

		void Initialize(uint32_t layers, uint32_t mips, const ImageState &initial, uint32_t faces = 1);

		ImageState &Get(uint32_t layer, uint32_t mip);

		const ImageState &Get(uint32_t layer, uint32_t mip) const;
	};
} // namespace BHive