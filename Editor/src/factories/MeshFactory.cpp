#include "mesh/MeshImportData.h"
#include "mesh/MeshImporter.h"
#include "mesh/SkeletalMesh.h"
#include "MeshFactory.h"
#include "subsystem/SubSystem.h"
#include "subsystems/WindowSubSystem.h"
#include "windows/editors/MeshOptionsWindow.h"

namespace BHive
{

	Ref<Asset> MeshFactory::Import(const std::filesystem::path &path)
	{
		MeshImporter importer;
		auto import_data = importer.Import(path);

		auto &window_system = SubSystemContext::Get().GetSubSystem<WindowSubSystem>();
		auto window =
			window_system.AddWindow<MeshOptionsWindow>(this, FMeshImportOptions{path, import_data});
		return nullptr;
	}

	REFLECT_FACTORY(MeshFactory, StaticMesh, ".gltf", ".glb")
} // namespace BHive