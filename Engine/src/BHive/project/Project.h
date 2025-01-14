#pragma once

#include "core/Core.h"
#include "reflection/Reflection.h"
#include "serialization/Serialization.h"

namespace BHive
{
	class StreamReader;
	class StreamWriter;

	struct FProjectConfiguration
	{
		std::string mName = "Untitled";
		std::filesystem::path mProjectDirectory;
		std::filesystem::path mResourcesDirectory;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(MAKE_NVP("Name", mName), MAKE_NVP("ProjectPath", mProjectDirectory),
			   MAKE_NVP("ResourcesPath", mResourcesDirectory));
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
			return sActiveProject->mConfig.mName;
		}

		static std::filesystem::path GetProjectDirectory()
		{
			ASSERT(sActiveProject);
			return sActiveProject->mConfig.mProjectDirectory;
		}

		static std::filesystem::path GetResourceDirectory()
		{
			ASSERT(sActiveProject);
			return GetProjectDirectory() / sActiveProject->mConfig.mResourcesDirectory;
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
