#pragma once

#include "core/Core.h"
#include "core/UUID.h"

namespace BHive
{
	class StreamReader;
	class StreamWriter;
	class World;

	struct BHIVE_API FProjectConfiguration
	{
		std::string Name = "Untitled";
		std::filesystem::path ProjectDirectory;
		std::filesystem::path ResourcesDirectory;
		UUID StartScene = NullID;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(MAKE_NVP(Name), MAKE_NVP(ProjectDirectory), MAKE_NVP(ResourcesDirectory), MAKE_NVP(StartScene));
		}

		REFLECTABLE()
	};

	REFLECT_EXTERN(FProjectConfiguration)

	class BHIVE_API Project
	{
	public:
		Project() = default;
		Project(const FProjectConfiguration &config);

		static Ref<Project> New(const FProjectConfiguration &config);

		static Ref<Project> GetActive() { return sActiveProject; }

		static Ref<Project> LoadProject(const std::filesystem::path &path);

		static bool SaveProject();

		static bool SaveProject(const FProjectConfiguration &config);

		static const char *GetFileFilter() { return "BHProj (*bhproj)\0 *.bhproj\0"; }

		static const char *GetExtension() { return ".bhproj"; }

		static std::string GetProjectName();

		static std::filesystem::path GetProjectDirectory();

		static std::filesystem::path GetResourceDirectory();

		static std::filesystem::path GetModulePath();

		static std::filesystem::path GetResourceRelativePath(const std::filesystem::path &path);

		static FProjectConfiguration &GetConfiguration();

	private:
		FProjectConfiguration mConfig;
		static inline Ref<Project> sActiveProject;
	};

} // namespace BHive
