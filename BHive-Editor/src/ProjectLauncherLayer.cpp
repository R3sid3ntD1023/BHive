#include "core/FileDialog.h"
#include "core/serialization/Serialization.h"
#include "core/threading/Threading.h"
#include "EditorLayer.h"
#include "gui/ImGuiExtended.h"
#include "project/Project.h"
#include "ProjectLauncherLayer.h"
#include "inspectors/Inspect.h"
#include "gui/Gui.h"
#include <Windows.h>

namespace BHive
{

	void ProjectLauncherLayer::OnAttach()
	{
		auto &app = Application::Get();
		auto &cmd = app.GetSpecification().CommandLine;

		std::ifstream in("Settings.projlncher", std::ios::in);
		if (in)
		{
			cereal::BinaryInputArchive ar(in);
			ar(mSettings);
		}
	}

	void ProjectLauncherLayer::OnDetach()
	{

		std::ofstream out("Settings.projlncher", std::ios::out | std::ios::binary);
		if (out)
		{

			cereal::BinaryOutputArchive ar(out);
			ar(mSettings);
			return;
		}

		DWORD error_message = GetLastError();
		LPSTR message_buffer = nullptr;
		size_t size = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error_message,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message_buffer, 0, NULL);

		std::string message(message_buffer, size);
		LocalFree(message_buffer);

		LOG_ERROR(message);
	}

	void ProjectLauncherLayer::OnGuiRender()
	{
		static FProjectConfiguration project_configuration{"Untitled", "", "resources"};

		auto dock_name = GUI::GetDockSpaceID();
		ImGuiID dockspace_id = ImGui::GetID(dock_name);

		ImGui::DockBuilderDockWindow("ProjectLauncher", dockspace_id);

		auto flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
					 ImGuiWindowFlags_NoTitleBar;
		if (ImGui::Begin("ProjectLauncher", nullptr, flags))
		{
			ImGui::BeginTable("##projectlauncher", 2, ImGuiTableFlags_BordersInnerV);
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			auto path_str = project_configuration.ProjectDirectory.string();
			auto resoucre_str = project_configuration.ResourcesDirectory.string();
			auto name = project_configuration.Name;

			ImGui::TableNextColumn();

			for (auto &[name, path] : mSettings.mRecentProjectPaths)
			{
				if (ImGui::Button(name.c_str(), {200, 200}))
				{
					OpenProject(path);
				}
			}

			ImGui::EndTable();

			Inspect::inspect("Project Name", project_configuration.Name, false, false);
			Inspect::inspect("Project Directory", project_configuration.ProjectDirectory, false, false);
			Inspect::inspect("Resource Directory", project_configuration.ResourcesDirectory, false, false);

			if (ImGui::Button("Create"))
			{
				CreateProject(project_configuration, mMessage);
			}

			ImGui::SameLine();

			if (ImGui::Button("Open"))
			{
				auto path_str = FileDialogs::OpenFile("Project (*.proj)\0*.proj\0");
				if (!path_str.empty())
				{
					OpenProject(path_str);
				}
			}

			ImGui::TextUnformatted(mMessage.c_str());
		}

		ImGui::End();
	}

	void ProjectLauncherLayer::OpenProject(const std::filesystem::path &path)
	{
		/*STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		auto command_line = "C:/users/dariu/Documents/BHive/bin/Windows/Debug/Editor.exe " + path.string();
		CreateProcess(
			"C:/users/dariu/Documents/BHive/bin/Windows/Debug/Editor.exe ", command_line.data(), NULL, NULL, FALSE, 0, NULL,
			NULL, &si, &pi);*/

		mSettings.mRecentProjectPaths.emplace(path.stem().string(), path);
		Project::LoadProject(path);

		Application::Get().SubmitToMainQueue(
			[this]()
			{
				auto &app = Application::Get();
				app.PopLayer(this);
				app.PushLayer<EditorLayer>();
			});
	}

	void ProjectLauncherLayer::CreateProject(const FProjectConfiguration &config, std::string &message)
	{
		auto path = config.ProjectDirectory / (config.Name + ".proj");
		if (std::filesystem::exists(path))
		{
			message = "Project Already exists";
			return;
		}

		if (!std::filesystem::exists(config.ProjectDirectory))
		{
			std::filesystem::create_directory(config.ProjectDirectory);
		}

		std::ofstream out(path, std::ios::out);

		if (!out)
			return;

		cereal::JSONOutputArchive ar(out);
		const_cast<FProjectConfiguration &>(config).Serialize(ar);

		message = std::format("Created {} project sucessfully!", config.Name);

		mSettings.mRecentProjectPaths.emplace(config.Name, path);

		OpenProject(path);
	}

} // namespace BHive
