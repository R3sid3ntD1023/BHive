#include "TerrainMaterial.h"

namespace BHive
{
	TerrainMaterial::TerrainMaterial()
		: Material("TerrainMaterial.glsl")
	{
	}

	void TerrainMaterial::Save(cereal::BinaryOutputArchive &ar) const
	{
	}

	void TerrainMaterial::Load(cereal::BinaryInputArchive &ar)
	{
	}

	REFLECT(TerrainMaterial)
	{
		BEGIN_REFLECT(TerrainMaterial);
	}
} // namespace BHive