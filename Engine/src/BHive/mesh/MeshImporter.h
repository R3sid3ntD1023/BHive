#pragma once

#include "core/Core.h"
#include "asset/AssetMetaData.h"
#include "MeshImportData.h"

namespace BHive
{
	class StaticMesh;
	class SkeletalMesh;
	class SkeletalAnimation;

	struct MeshImporter
	{
		static bool Import(const std::filesystem::path &path, FMeshImportData &data);
	};
} // namespace BHive