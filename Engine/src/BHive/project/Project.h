#pragma once

#include "core/Core.h"
#include "core/reflection/Reflection.h"
#include "core/serialization/Serialization.h"

namespace BHive
{
	class StreamReader;
	class StreamWriter;

	struct FProjectConfiguration
	{
		std::string Name = "Untitled";
		std::filesystem::path ProjectDirectory;
		std::filesystem::path ResourcesDirectory;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(MAKE_NVP(Name), MAKE_NVP(ProjectDirectory), MAKE_NVP(ResourcesDirectory));
		}

		REFLECTABLE()
	};

	class Project
	{
	public:
		Project() = default;
		Project(const FProjectConfiguration &config);

		static Ref<Project> New(const FProjectConfiguration &config);
		static Ref<Project> GetActive() { return sActiveProject; }
		static Ref<Project> LoadProject(const std::filesystem::path &path);
		static bool SaveProject(const std::filesystem::path &path);

		static std::string GetProjectName()
		{
			ASSERT(sActiveProject);
			return sActiveProject->mConfig.Name;
		}

		static std::filesystem::path GetProjectDirectory()
		{
			ASSERT(sActiveProject);
			return sActiveProject->mConfig.ProjectDirectory;
		}

		static std::filesystem::path GetResourceDirectory()
		{
			ASSERT(sActiveProject);
			return GetProjectDirectory() / sActiveProject->mConfig.ResourcesDirectory;
		}

		static FProjectConfiguration &GetConfiguration()
		{
			ASSERT(sActiveProject);
			return sActiveProject->mConfig;
		}

	private:
		FProjectConfiguration mConfig;
		static inline Ref<Project> sActiveProject;
	};
} // namespace BHive
