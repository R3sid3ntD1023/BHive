#include "core/EntryPoint.h"
#include "RuntimeLayer.h"
#include "SceneLayer.h"
#include "WorldLayer.h"
#include "InspectorCore.h"

namespace BHive
{
	class RuntimeApp : public Application
	{
	public:
		RuntimeApp(const FApplicationSpecification &specs)
			: Application(specs)
		{
			// PushLayer<SceneLayer>();
			PushLayer<WorldLayer>();
			//    PushLayer<PerformanceLayer>();
			//   PushLayer<RuntimeLayer>();
		}

		void OnPostInitialize() override
		{
			Application::OnPostInitialize();

			inspectors::SetImGuiContext(GetImGuiLayer()->GetContext());
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
		specs.VSync = EngineConfig::EnableVSync;
		return new RuntimeApp(specs);
	}
} // namespace BHive