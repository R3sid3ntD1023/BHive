#include "core/EntryPoint.h"

namespace BHive
{
	class RuntimeApp : public Application
	{
	public:
		RuntimeApp(const FApplicationSpecification &specs)
			: Application(specs)
		{
		}
	};

	Application *CreateApplication(const FCommandLine &cmd)
	{
		FApplicationSpecification specs{};
		specs.CommandLine = cmd;
		specs.CenterWindow = true;
		specs.Maximize = true;
		specs.Title = "BHive Runtime";
		specs.Size = {1280, 720};
		specs.Flags = 0;
		return new RuntimeApp(specs);
	}
} // namespace BHive