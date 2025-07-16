#pragma once

#include "WindowBase.h"
#include "mesh/MeshImportResolver.h"

namespace BHive
{

	class MeshFactory;

	class MeshOptionsWindow : public WindowBase
	{
	public:
		MeshOptionsWindow(MeshFactory *factory, const std::filesystem::path &filePath, const FMeshImportData &data);

		virtual void OnGuiRender() final override;

		bool ShouldClose() const override { return WindowBase::ShouldClose() || mShouldClose; };

	protected:
		virtual const char *GetName() const { return "Mesh Import"; }

	private:
		bool mIsOpen = true;
		bool mShouldClose{false};

		FMeshImportData mImportData;
		FMeshImportOptions mOptions;
		std::filesystem::path mImportPath;

		MeshFactory *mFactory = nullptr;
	};

} // namespace BHive
