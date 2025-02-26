#include "core/EntryPoint.h"
#include "SandboxLayer.h"

namespace BHive
{
	class Sandbox : public Application
	{
	public:
		Sandbox(const FApplicationSpecification &spec)
			: Application(spec)
		{
			PushLayer(new SandboxLayer());
		}

	private:
	};

	Application *CreateApplication(const FCommandLine &cmd)
	{
		FApplicationSpecification specs{};
		specs.Size = {800, 600};
		specs.Title = "Shadows";
		specs.CommandLine = cmd;

		return new Sandbox(specs);
	}

} // namespace BHive