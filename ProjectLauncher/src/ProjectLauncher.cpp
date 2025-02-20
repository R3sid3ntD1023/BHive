#include "core/EntryPoint.h"
#include "ProjectLauncherLayer.h"

namespace BHive
{
	class ProjectLauncher : public Application
	{
	public:
		ProjectLauncher(const FApplicationSpecification &specification)
			: Application(specification)
		{
			PushLayer(new ProjectLauncherLayer());
		}
	};

	Application *CreateApplication(const FCommandLine &cmd)
	{
		FApplicationSpecification specs;
		specs.CommandLine = cmd;
		specs.Size = {800, 600};
		specs.Title = "Project Launcher";
		return new ProjectLauncher(specs);
	}
} // namespace BHive
