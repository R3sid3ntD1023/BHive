#include "ProjectLauncherLayer.h"
#include "project/Project.h"
#include "gui/ImGuiExtended.h"
#include "gui/Gui.h"
#include "serialization/Serialization.h"
#include "core/FileDialog.h"
#include "project/Project.h"
#include "serialization/FileStream.h"

namespace ImGui
{
    using FileSystemCallback = std::string (*)();

    bool InputText(const char *label, std::filesystem::path *path,
                   ImGuiInputTextFlags flags = 0, FileSystemCallback filecallback = (FileSystemCallback) nullptr,
                   ImGuiInputTextCallback callback = (ImGuiInputTextCallback) nullptr,
                   void *user_data = (void *)nullptr)
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
    void FProjectLauncherSettings::Serialize(StreamWriter &ar) const
    {
        ar(mRecentProjectPaths);
    }

    void FProjectLauncherSettings::Deserialize(StreamReader &ar)
    {
        ar(mRecentProjectPaths);
    }

    std::string GetDirectory()
    {
        return FileDialogs::GetDirectory();
    }

    void ProjectLauncherLayer::OnAttach()
    {
        auto &app = Application::Get();
        auto &cmd = app.GetSpecification().CommandLine;

        FileStreamReader ar("launcher/Settings.projlncher");
        if (ar)
        {
            ar(mSettings);
        }

        auto &io = ImGui::GetIO();
        io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
    }

    void ProjectLauncherLayer::OnDetach()
    {
        FileStreamWriter ar("launcher/Settings.projlncher");
        if (ar)
        {
            ar(mSettings);
        }
    }

    void ProjectLauncherLayer::OnGuiRender(float)
    {
        static FProjectConfiguration project_configuration{"Untitled", "", "resources"};

        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        auto window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
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

            ImGui::InputLayout("Project Name", []()
                               { return ImGui::InputText("##ProjectName", &project_configuration.mName, ImGuiInputTextFlags_EnterReturnsTrue); });

            ImGui::InputLayout("Directory Path", []()
                               { return ImGui::InputText("##DirectoryPath", &project_configuration.mProjectDirectory, ImGuiInputTextFlags_EnterReturnsTrue, GetDirectory); });

            ImGui::InputLayout("Resource Directory", []()
                               { return ImGui::InputText("##ResourceDirectory", &project_configuration.mResourcesDirectory, ImGuiInputTextFlags_EnterReturnsTrue, GetDirectory); });

            if (ImGui::Button("Create"))
            {
                CreateProject(project_configuration);
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
        CreateProcess("C:/users/dariu/Documents/BHive/bin/Windows/Debug/Editor.exe ",
                      command_line.data(),
                      NULL,
                      NULL,
                      FALSE,
                      0,
                      NULL,
                      NULL,
                      &si,
                      &pi);

        mSettings.mRecentProjectPaths.emplace(path.stem().string(), path);

        Application::Get().Close();
    }

    void ProjectLauncherLayer::CreateProject(const FProjectConfiguration &config)
    {
        if (std::filesystem::exists(config.mProjectDirectory / config.mName))
        {
            LOG_ERROR("Project Already exists");
            return;
        }

        std::filesystem::create_directory(config.mProjectDirectory / config.mName);
    }

} // namespace BHive
