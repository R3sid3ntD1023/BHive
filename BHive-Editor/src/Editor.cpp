#include "core/Application.h"
#include "core/EntryPoint.h"
#include "EditorLayer.h"
#include "ProjectLauncherLayer.h"
#include "Inspectors.h"
#include "gui/Gui.h"

namespace BHive
{
	class BHiveEditor : public Application
	{
	public:
		BHiveEditor(const FApplicationSpecification &spec)
			: Application(spec)
		{
			SetCurrentContext(ImGui::GetCurrentContext());

			PushLayer(new ProjectLauncherLayer());
		}

		void OnBeginGUIRender() override
		{
			Application::OnBeginGUIRender();

			GUI::BeginDockSpace("DockSpace", nullptr, 10.f);
		}

		void OnEndGUIRender() override
		{
			GUI::EndDockSpace();

			Application::OnEndGUIRender();
		}
	};

	Application *CreateApplication(const FCommandLine &cmd)
	{
		FApplicationSpecification spec{};
		spec.Title = "BHive Editor";
		spec.CommandLine = cmd;
		spec.Size = {1280, 720};
		spec.Maximize = true;
		return new BHiveEditor(spec);
	}
} // namespace BHive