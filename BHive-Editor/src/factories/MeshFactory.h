#pragma once

#include "asset/Factory.h"
#include "mesh/MeshImportData.h"

namespace BHive
{

	class BHIVE MeshFactory : public Factory
	{
	public:
		virtual Ref<Asset> Import(const std::filesystem::path &path) override;

		virtual std::vector<Ref<Asset>> GetOtherCreatedAssets() override { return mOtherAssets; }

		virtual std::vector<std::string> GetSupportedExtensions() { return {".gltf", ".glb"}; }

		virtual std::string GetDisplayName() const { return "Mesh"; }

		REFLECTABLEV(Factory)

	private:
		std::vector<Ref<Asset>> mOtherAssets;

		friend class MeshOptionsWindow;
	};
} // namespace BHive
