#include "core/Application.h"
#include "core/EntryPoint.h"
#include "EditorLayer.h"
#include "ProjectLauncherLayer.h"
#include "gui/Gui.h"
#include "core/layers/ImGuiLayer.h"
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

			PushLayer(new ProjectLauncherLayer());

			if (std::filesystem::exists(EDITOR_CONFIG_FILE))
			{
				mINI::INIStructure ini;
				auto config = mINI::INIFile(EDITOR_CONFIG_FILE);
				config.read(ini);

				if (ini.has("Editor"))
				{
					auto settings = ini.get("Editor");
					if (settings.has("Layout"))
					{
						std::string filename = "";
						std::istringstream(settings.get("Layout")) >> filename;
						auto layout = std::filesystem::path(EDITOR_RESOURCE_PATH) / ("layouts/" + filename);
						ImGui::LoadIniSettingsFromDisk(layout.string().c_str());
					}
				}
			}
		}

		~BHiveEditor() {}

		void OnBeginGUIRender() override
		{
			Application::OnBeginGUIRender();

			GUI::BeginDockSpace("DockSpace", nullptr);
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
		spec.VSync = false;

		mINI::INIStructure ini;

		if (std::filesystem::exists(EDITOR_CONFIG_FILE))
		{
			auto config = mINI::INIFile(EDITOR_CONFIG_FILE);
			config.read(ini);
		}

		if (ini.has("Application"))
		{
			char delimiter;

			auto settings = ini.get("Application");
			if (settings.has("Size"))
			{
				std::istringstream(settings.get("Size")) >> spec.Size.x >> delimiter >> spec.Size.y;
			}

			if (settings.has("Maximize"))
			{
				std::istringstream(settings.get("Maximize")) >> spec.Maximize;
			}

			if (settings.has("Vsync"))
			{
				std::istringstream(settings.get("VSync")) >> spec.VSync;
			}
		}

		return new BHiveEditor(spec);
	}
} // namespace BHive