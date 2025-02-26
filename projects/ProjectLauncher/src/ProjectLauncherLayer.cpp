#include "core/FileDialog.h"
#include "gui/ImGuiExtended.h"
#include "project/Project.h"
#include "ProjectLauncherLayer.h"
#include "core/serialization/Serialization.h"
#include <Windows.h>

namespace ImGui
{
	using FileSystemCallback = std::string (*)();

	bool InputText(
		const char *label, std::filesystem::path *path, ImGuiInputTextFlags flags = 0,
		FileSystemCallback filecallback = (FileSystemCallback) nullptr,
		ImGuiInputTextCallback callback = (ImGuiInputTextCallback) nullptr, void *user_data = (void *)nullptr)
	{
		auto str = path->string();
		if (InputText(label, &str, flags, callback, user_data))
		{
			*path = str;
			return true;
		}

		ImGui::SameLine();

		if (ImGui::Button("..."))
		{
			if (filecallback)
			{
				auto path_str = filecallback();
				if (!path_str.empty())
				{
					*path = path_str;
					return true;
				}
			}
		}

		return false;
	}

	template <typename TFunc, typename... TArgs>
	bool InputLayout(const char *label, TFunc func, TArgs &&...args)
	{
		ImGui::BeginTable("##colums", 2);
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);

		ImGui::TableNextColumn();

		bool changed = func(std::forward<TArgs>(args)...);

		ImGui::EndTable();

		return changed;
	}
} // namespace ImGui

namespace BHive
{

	std::string GetDirectory()
	{
		return FileDialogs::GetDirectory();
	}

	void ProjectLauncherLayer::OnAttach()
	{
		auto &app = Application::Get();
		auto &cmd = app.GetSpecification().CommandLine;

		std::ifstream in("Settings.projlncher", std::ios::in);
		if (in)
		{
			cereal::BinaryInputArchive ar(in);
			mSettings.Serialize(ar);
		}

		auto &io = ImGui::GetIO();
		io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
	}

	void ProjectLauncherLayer::OnDetach()
	{

		std::ofstream out("Settings.projlncher", std::ios::out | std::ios::binary);
		if (out)
		{

			cereal::BinaryOutputArchive ar(out);
			mSettings.Serialize(ar);
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

		const ImGuiViewport *viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		auto window_flags =
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (ImGui::Begin("ProjectLauncher", nullptr, window_flags))
		{
			ImGui::BeginTable("##projectlauncher", 2, ImGuiTableFlags_BordersInnerV);
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			auto path_str = project_configuration.mProjectDirectory.string();
			auto resoucre_str = project_configuration.mResourcesDirectory.string();
			auto name = project_configuration.mName;

			ImGui::TableNextColumn();

			for (auto &[name, path] : mSettings.mRecentProjectPaths)
			{
				if (ImGui::Button(name.c_str(), {200, 200}))
				{
					OpenProject(path);
				}
			}

			ImGui::EndTable();

			ImGui::InputLayout(
				"Project Name",
				[]() {
					return ImGui::InputText(
						"##ProjectName", &project_configuration.mName, ImGuiInputTextFlags_EnterReturnsTrue);
				});

			ImGui::InputLayout(
				"Directory Path",
				[]()
				{
					return ImGui::InputText(
						"##DirectoryPath", &project_configuration.mProjectDirectory, ImGuiInputTextFlags_EnterReturnsTrue,
						GetDirectory);
				});

			ImGui::InputLayout(
				"Resource Directory",
				[]()
				{
					return ImGui::InputText(
						"##ResourceDirectory", &project_configuration.mResourcesDirectory,
						ImGuiInputTextFlags_EnterReturnsTrue, GetDirectory);
				});

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

		ImGui::PopStyleVar(2);
	}

	void ProjectLauncherLayer::OpenProject(const std::filesystem::path &path)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		auto command_line = "C:/users/dariu/Documents/BHive/bin/Windows/Debug/Editor.exe " + path.string();
		CreateProcess(
			"C:/users/dariu/Documents/BHive/bin/Windows/Debug/Editor.exe ", command_line.data(), NULL, NULL, FALSE, 0, NULL,
			NULL, &si, &pi);

		mSettings.mRecentProjectPaths.emplace(path.stem().string(), path);

		Application::Get().Close();
	}

	void ProjectLauncherLayer::CreateProject(const FProjectConfiguration &config, std::string &message)
	{
		auto path = config.mProjectDirectory / (config.mName + ".proj");
		if (std::filesystem::exists(path))
		{
			message = "Project Already exists";
			return;
		}

		if (!std::filesystem::exists(config.mProjectDirectory))
		{
			std::filesystem::create_directory(config.mProjectDirectory);
		}

		std::ofstream out(path, std::ios::out);

		if (!out)
			return;

		cereal::JSONOutputArchive ar(out);
		const_cast<FProjectConfiguration &>(config).Serialize(ar);

		message = std::format("Created {} project sucessfully!", config.mName);

		mSettings.mRecentProjectPaths.emplace(config.mName, path);
	}

} // namespace BHive
