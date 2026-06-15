#pragma once

#pragma warning(push, 0)
#include "msdf-atlas-gen/msdf-atlas-gen.h"
#pragma warning(pop)

namespace BHive
{
	struct MSDFData
	{
		std::vector<msdf_atlas::GlyphGeometry> Glyphs;
		msdf_atlas::FontGeometry FontGeometry;
	};

} // namespace BHive