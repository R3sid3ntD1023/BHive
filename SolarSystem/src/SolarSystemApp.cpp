#include <core/Application.h>
#include <core/EntryPoint.h>
#include "SolarSystemLayer.h"

struct TestApplication : public BHive::Application
{
	TestApplication(const BHive::FApplicationSpecification &specs)
		: Application(specs)
	{
		PushLayer(new SolarSystem::SolarSystemLayer());
	}
};

namespace BHive
{
	Application *CreateApplication(const FCommandLine &cmdline)
	{
		FApplicationSpecification specs{};
		specs.Width = 800;
		specs.Height = 600;
		specs.Title = "TestApp-Water";
		return new TestApplication(specs);
	}

} // namespace BHive