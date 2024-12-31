#pragma once

#include "asset/Factory.h"
#include "mesh/MeshImportData.h"

namespace BHive
{
	struct FMeshImportOptions
	{
		std::filesystem::path mPath;
		FMeshImportData mData;
	};

	class BHIVE MeshFactory : public Factory
	{
	public:
		virtual bool Import(Ref<Asset> &asset, const std::filesystem::path &path);

		REFLECTABLEV(Factory)
	};

	class SkeletonFactory : public Factory
	{
	public:
		virtual bool Import(Ref<Asset> &asset, const std::filesystem::path &path);

		REFLECTABLEV(Factory)
	};

	class AnimationFactory : public Factory
	{
	public:
		virtual bool Import(Ref<Asset> &asset, const std::filesystem::path &path);

		REFLECTABLEV(Factory)
	};

} // namespace BHive
