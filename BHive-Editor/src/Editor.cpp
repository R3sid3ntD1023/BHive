#include "core/Application.h"
#include "core/EntryPoint.h"
#include "gui/GUICore.h"
#include "core/layers/ImGuiLayer.h"
#include "ProjectLauncherLayer.h"
#include "project/Project.h"
#include <mini/ini.h>
#include <ImGuizmo.h>

#define EDITOR_CONFIG_FILE "EditorConfig.ini"

namespace BHive
{
	class BHiveEditor : public Application
	{
	public:
		BHiveEditor(const FApplicationSpecification &spec)
			: Application(spec)
		{
			SetImGuiContext();

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

		void SetImGuiContext()
		{
			ImGuiMemAllocFunc alloc_func = nullptr;
			ImGuiMemFreeFunc free_func = nullptr;
			void *user_data = nullptr;

			ImGuiLayer &layer = GetImGuiLayer();
			auto ctx = layer.GetContext();

			layer.GetAllocatorCallbacks(&alloc_func, &free_func, &user_data);

			ImGui::SetCurrentContext((ImGuiContext *)ctx);
			ImGui::SetAllocatorFunctions(alloc_func, free_func, user_data);
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

		return new BHiveEditor(spec);
	}
} // namespace BHive