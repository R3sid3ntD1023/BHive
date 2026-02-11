#include "core/EntryPoint.h"
#include "RuntimeLayer.h"

namespace BHive
{
	class RuntimeApp : public Application
	{
	public:
		RuntimeApp(const FApplicationSpecification &specs)
			: Application(specs)
		{
			PushLayer(new RuntimeLayer());
		}
	};

	Application *CreateApplication(const FCommandLine &cmd)
	{
		FApplicationSpecification specs{};
		specs.CommandLine = cmd;
		specs.CenterWindow = true;
		specs.Title = "BHive Runtime";
		specs.Size = {800, 600};
		specs.Flags = /*EApplicationFlags::EnableImGui |*/ EApplicationFlags::EnableRendering;
		return new RuntimeApp(specs);
	}
} // namespace BHive