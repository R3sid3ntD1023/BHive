#include "ProjectLauncherLayer.h"
#include "core/threading/Threading.h"
#include "gui/ImGuiExtended.h"
#include "project/Project.h"
#include "inspectors/Inspect.h"
#include "gui/Gui.h"
#include "core/Application.h"
#include "core/platform/Platform.h"
#include "EditorLayer.h"
#include "gfx/Texture.h"
#include "importers/TextureImporter.h"

namespace BHive
{
	constexpr const char *cSettingsFileName = "Settings.projlncher";

	void ProjectLauncherLayer::OnAttach()
	{
		auto &app = Application::Get();
		auto &cmd = app.GetSpecification().CommandLine;

		if (std::filesystem::exists(cSettingsFileName))
		{
			std::ifstream in(cSettingsFileName, std::ios::in);
			try
			{
				cereal::BinaryInputArchive ar(in);
				ar(mSettings);
			}
			catch (const std::exception &e)
			{
				LOG_ERROR("{}", e.what())
			}
		}

		for (auto &[name, path] : mSettings.mRecentProjectPaths)
		{
			auto snapshot_path = path.parent_path() / "snapshot.png";
			if (std::filesystem::exists(snapshot_path))
				mSnapshots[name] = TextureLoader::Import(snapshot_path);
		}
	}

	void ProjectLauncherLayer::OnDetach()
	{

		std::ofstream out(cSettingsFileName, std::ios::out | std::ios::binary);
		try
		{

			cereal::BinaryOutputArchive ar(out);
			ar(mSettings);
			return;
		}
		catch (const std::exception &e)
		{
			LOG_ERROR("{}", e.what())
		}
	}

	void ProjectLauncherLayer::OnGuiRender()
	{
		static FProjectConfiguration project_configuration{"Untitled", "", "resources"};
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

		const ImGuiViewport *viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin("ProjectLauncher", nullptr, window_flags);

		ImGui::PopStyleVar(2);

		// display recent projects
		auto size = ImGui::GetContentRegionAvail().x;
		float thumbnail_size = 256.f;

		if (auto count = mSettings.mRecentProjectPaths.size())
		{
			int columns = floor(size / thumbnail_size);

			ImGui::BeginTable("##projectlauncher", columns);
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			for (auto &[name, path] : mSettings.mRecentProjectPaths)
			{
				bool clicked = false;

				if (mSnapshots.contains(name))
				{
					clicked = ImGui::ImageButton(name.c_str(), (ImTextureID)(uint64_t)(uint32_t)*mSnapshots.at(name), {thumbnail_size}, {0, 1}, {1, 0});
				}
				else
				{
					clicked = ImGui::Button(name.c_str(), thumbnail_size);
				}

				if (clicked)
					OpenProject(path);

				ImGui::TableNextColumn();
			}

			ImGui::EndTable();
		}

		Inspect::get().inspect("Project Name", project_configuration, project_configuration.Name, false, false);
		Inspect::get().inspect("Project Directory", project_configuration, project_configuration.ProjectDirectory, false, false);
		Inspect::get().inspect("Resource Directory", project_configuration, project_configuration.ResourcesDirectory, false, false);

		if (ImGui::Button("Create"))
		{
			CreateProject(project_configuration, mMessage);
		}

		ImGui::SameLine();

		if (ImGui::Button("Open"))
		{
			if (auto info = Platform::OpenFile(Project::GetFileFilter()))
			{
				OpenProject(info);
			}
		}

		ImGui::TextUnformatted(mMessage.c_str());

		ImGui::End();
	}

	void ProjectLauncherLayer::OpenProject(const std::filesystem::path &path)
	{

		mSettings.mRecentProjectPaths.emplace(path.stem().string().c_str(), path);

		Thread::Dispatch(
			[=]()
			{
				Project::LoadProject(path);

				auto &app = Application::Get();
				app.PopLayer(this);
				app.PushLayer<EditorLayer>();
			});
	}

	void ProjectLauncherLayer::CreateProject(const FProjectConfiguration &config, std::string &message)
	{
		auto path = config.ProjectDirectory / (config.Name + Project::GetExtension());
		if (std::filesystem::exists(path))
		{
			message = "Project Already exists";
			return;
		}

		if (!std::filesystem::exists(config.ProjectDirectory))
		{
			std::filesystem::create_directory(config.ProjectDirectory);
		}

		if (Project::SaveProject(config))
		{
			message = "Created Project";
		}
	}

} // namespace BHive
