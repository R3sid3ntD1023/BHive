#include "Project.h"

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

		try
		{
			std::ifstream in(path, std::ios::in);
			cereal::JSONInputArchive ar(in);
			ar(sActiveProject->mConfig);
			sActiveProject->mConfig.ProjectDirectory = path.parent_path();
		}
		catch (const std::exception &e)
		{
			LOG_ERROR("Project Load ERROR : {}", e.what());
			return nullptr;
		}

		return sActiveProject;
	}

	bool Project::SaveProject()
	{
		ASSERT(sActiveProject);

		auto &config = sActiveProject->GetConfiguration();
		std::ofstream out(config.ProjectDirectory / (config.Name + ".proj"), std::ios::out);
		if (!out)
			return false;

		try
		{
			cereal::JSONOutputArchive ar(out);
			ar(config);

			LOG_INFO("Saved {} project sucessfully!", config.Name);
		}
		catch (const std::exception &e)
		{
			LOG_ERROR("Project Save ERROR : {}", e.what());
			return false;
		}

		return true;
	}

	bool Project::SaveProject(const FProjectConfiguration &config)
	{
		std::ofstream out(config.ProjectDirectory / (config.Name + ".proj"), std::ios::out);
		if (!out)
			return false;

		try
		{
			cereal::JSONOutputArchive ar(out);
			ar(config);

			LOG_INFO("Created {} project sucessfully!", config.Name);
		}
		catch (const std::exception &e)
		{
			LOG_ERROR("Project Save ERROR : {}", e.what());
			return false;
		}

		return true;
	}

	std::filesystem::path Project::GetResourceRelativePath(const std::filesystem::path &path)
	{
		return std::filesystem::relative(path, GetResourceDirectory());
	}

	REFLECT(FProjectConfiguration)
	{
		BEGIN_REFLECT(FProjectConfiguration)
		REFLECT_PROPERTY(Name)
		REFLECT_PROPERTY(ProjectDirectory)
		REFLECT_PROPERTY(ResourcesDirectory);
	}
} // namespace BHive