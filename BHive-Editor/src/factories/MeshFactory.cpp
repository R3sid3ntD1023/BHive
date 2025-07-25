#include "mesh/MeshImportData.h"
#include "mesh/MeshImporter.h"
#include "mesh/SkeletalMesh.h"
#include "mesh/StaticMesh.h"
#include "MeshFactory.h"
#include "core/subsystem/SubSystem.h"
#include "subsystems/WindowSubSystem.h"
#include "windows/MeshOptionsWindow.h"

namespace BHive
{

	Ref<Asset> MeshFactory::Import(const std::filesystem::path &path)
	{
		FMeshImportData data;
		if (!MeshImporter::Import(path, data))
			return nullptr;

		auto &window_system = SubSystemContext::Get().GetSubSystem<WindowSubSystem>();
		auto window = window_system.CreateWindow<MeshOptionsWindow>(this, path, data);
		return nullptr;
	}

	REFLECT_FACTORY(MeshFactory)
} // namespace BHive