#include "SolarSystemLayer.h"
#include <core/Application.h>
#include <core/EntryPoint.h>

struct UniverseApplication : public BHive::Application
{
	UniverseApplication(const BHive::FApplicationSpecification &specs)
		: Application(specs)
	{
		PushLayer(new SolarSystemLayer());
	}
};

namespace BHive
{
	Application *CreateApplication(const FCommandLine &cmdline)
	{
		FApplicationSpecification specs{};
		specs.Width = 800;
		specs.Height = 600;
		specs.Title = "Solar System";
		specs.VSync = false;
		return new UniverseApplication(specs);
	}

} // namespace BHive