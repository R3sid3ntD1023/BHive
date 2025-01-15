#include "Project.h"
#include "serialization/Serialization.h"

namespace BHive
{

	Project::Project(const FProjectConfiguration &config)
		: mConfig(config)
	{
	}

	Ref<Project> Project::New(const FProjectConfiguration &config)
	{
		sActiveProject = CreateRef<Project>(config);
		return sActiveProject;
	}

	Ref<Project> Project::LoadProject(const std::filesystem::path &path)
	{
		sActiveProject = CreateRef<Project>();

		std::ifstream in(path, std::ios::in);
		if (in)
		{
			cereal::JSONInputArchive ar(in);
			sActiveProject->mConfig.Serialize(ar);
		}

		return sActiveProject;
	}

	bool Project::SaveProject(const std::filesystem::path &path)
	{
		ASSERT(sActiveProject);

		std::ofstream out(path, std::ios::out);
		if (!out)
			return false;

		cereal::JSONOutputArchive ar(out);
		sActiveProject->mConfig.Serialize(ar);

		return true;
	}

	REFLECT(FProjectConfiguration)
	{
		BEGIN_REFLECT(FProjectConfiguration)
		REFLECT_PROPERTY("Name", mName)
		REFLECT_PROPERTY("ProjectDirectory", mProjectDirectory)
		REFLECT_PROPERTY("ResourcesDirectory", mResourcesDirectory);
	}
} // namespace BHive