#include "core/Application.h"
#include "core/EntryPoint.h"
#include "gui/GUICore.h"
#include "core/layers/ImGuiLayer.h"
#include "ProjectLauncherLayer.h"
#include "project/Project.h"
#include <mini/ini.h>

#define EDITOR_CONFIG_FILE "EditorConfig.ini"

namespace BHive
{
	class BHiveEditor : public Application
	{
	public:
		BHiveEditor(const FApplicationSpecification &spec)
			: Application(spec)
		{

			if (spec.CommandLine.Count > 1)
			{
				Project::LoadProject(spec.CommandLine[1]);
			}
			else
			{
				PushLayer<ProjectLauncherLayer>();
			}
		}

		~BHiveEditor() {}
	};

	Application *CreateApplication(const FCommandLine &cmd)
	{
		FApplicationSpecification spec{};
		spec.Title = "BHive Editor";
		spec.CommandLine = cmd;
		spec.Size = {1280, 720};
		spec.Maximize = true;
		spec.VSync = false;

		return new BHiveEditor(spec);
	}
} // namespace BHive