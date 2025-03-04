#include "SolarSystemLayer.h"
#include <core/Application.h>
#include <core/EntryPoint.h>

BEGIN_NAMESPACE(BHive)

struct UniverseApplication : public Application
{
	UniverseApplication(const FApplicationSpecification &specs)
		: Application(specs)
	{
		PushLayer(new SolarSystemLayer());
	}
};

Application *CreateApplication(const FCommandLine &cmdline)
{
	FApplicationSpecification specs{};
	specs.Size = {800, 600};
	specs.Title = "Solar System";
	specs.VSync = false;
	return new UniverseApplication(specs);
}

END_NAMESPACE