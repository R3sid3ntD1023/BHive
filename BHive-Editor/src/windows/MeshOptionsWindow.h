#pragma once

#include "ImWindowBase.h"
#include "mesh/MeshImportResolver.h"

namespace BHive
{

	class MeshFactory;

	class MeshOptionsWindow : public ImWindowBase
	{
	public:
		MeshOptionsWindow(MeshFactory *factory, const std::filesystem::path &filePath, const FMeshImportData &data);

		virtual void OnUpdateContent() final override;

		bool ShouldClose() const override { return ImWindowBase::ShouldClose() || mShouldClose; };

	protected:
		virtual const char *GetName() const override { return "Mesh Import"; }

	private:
		bool mShouldClose{false};

		FMeshImportData mImportData;
		FMeshImportOptions mOptions;
		std::filesystem::path mImportPath;

		MeshFactory *mFactory = nullptr;
	};

} // namespace BHive
