#include "mesh/MeshImportData.h"
#include "mesh/MeshImporter.h"
#include "mesh/SkeletalMesh.h"
#include "mesh/StaticMesh.h"
#include "MeshFactory.h"
#include "subsystem/SubSystem.h"
#include "subsystems/WindowSubSystem.h"
#include "windows/editors/MeshOptionsWindow.h"

namespace BHive
{

	Ref<Asset> MeshFactory::Import(const std::filesystem::path &path)
	{
		MeshImporter importer;
		FMeshImportData data;
		if (!importer.Import(path, data))
			return nullptr;

		auto &window_system = SubSystemContext::Get().GetSubSystem<WindowSubSystem>();
		auto window = window_system.AddWindow<MeshOptionsWindow>(this, FMeshImportOptions{path, data});
		return nullptr;
	}

	REFLECT_FACTORY(MeshFactory, StaticMesh, ".gltf", ".glb")
} // namespace BHive