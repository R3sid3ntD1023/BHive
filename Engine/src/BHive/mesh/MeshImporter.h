#pragma once

#include "asset/AssetMetaData.h"
#include "MeshImportData.h"

namespace BHive
{
	class StaticMesh;
	class SkeletalMesh;
	class SkeletalAnimation;

	struct MeshImporter
	{
		bool Import(const std::filesystem::path &path, FMeshImportData &data);
	};
} // namespace BHive