#pragma once

#include "msdf-atlas-gen/msdf-atlas-gen.h"

namespace BHive
{
	struct MSDFData
	{
		std::vector<msdf_atlas::GlyphGeometry> Glyphs;
		msdf_atlas::FontGeometry FontGeometry;
	};

} // namespace BHive