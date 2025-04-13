#include "core/EntryPoint.h"
#include "TestLayer.h"

namespace BHive
{
	class VulkanTestApp : public Application
	{
	public:
		VulkanTestApp(const FApplicationSpecification &specs)
			: Application(specs)
		{
			PushLayer(new TestLayer());
		}
	};

	Application *CreateApplication(const FCommandLine &cmd)
	{
		FApplicationSpecification specs{};
		specs.Title = "VulkanTest";
		specs.Size = {800, 600};
		specs.CommandLine = cmd;

		return new VulkanTestApp(specs);
	}
} // namespace BHive