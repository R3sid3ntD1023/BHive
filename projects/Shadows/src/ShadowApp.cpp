#include "core/EntryPoint.h"
#include "SandboxLayer.h"
#include "ReflectionLayer.h"

#define SHADOWS 0
#define REFLECTION 1

namespace BHive
{
	class Sandbox : public Application
	{
	public:
		Sandbox(const FApplicationSpecification &spec)
			: Application(spec)
		{
#if SHADOWS
			PushLayer(new SandboxLayer());
#endif

#if REFLECTION
			PushLayer(new ReflectionLayer());
#endif
		}

	private:
	};

	Application *CreateApplication(const FCommandLine &cmd)
	{
		FApplicationSpecification specs{};
		specs.Size = {800, 600};
		specs.Title = "TestApp";
		specs.CommandLine = cmd;

		return new Sandbox(specs);
	}

} // namespace BHive