#include "PerformanceLayer.h"
#include "RuntimeLayer.h"
#include "SceneLayer.h"
#include "core/EntryPoint.h"

namespace BHive
{
	class RuntimeApp : public Application
	{
	public:
		RuntimeApp(const FApplicationSpecification &specs)
			: Application(specs)
		{
			PushLayer<SceneLayer>();
			PushLayer<PerformanceLayer>();
			//    PushLayer<RuntimeLayer>();
		}
	};

	Application *CreateApplication(const FCommandLine &cmd)
	{
		EngineConfig::Load(ENGINE_PATH "/../Config/Engine.ini");

		FApplicationSpecification specs{};
		specs.CommandLine = cmd;
		specs.CenterWindow = true;
		specs.Maximize = EngineConfig::Maximized;
		specs.Title = "BHive Runtime";
		specs.Size = {EngineConfig::WindowWidth, EngineConfig::WindowHeight};
		return new RuntimeApp(specs);
	}
} // namespace BHive