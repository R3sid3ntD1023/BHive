#pragma once

namespace BHive
{
	enum class EMaterialTextureType
	{
		Unknown,
		Albedo,
		Diffuse,
		Normal,
		Metalness,
		Specular,
		Shininess,
		Roughness,
		Emission,
		Opacity,
		Height
	};

	enum class EMaterialShadingMode
	{
		Lambert,
		Standard
	};
} // namespace BHive