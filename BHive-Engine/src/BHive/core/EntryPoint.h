#pragma once

#include "Application.h"
#include "audio/AudioContext.h"
#include "core/layers/ImGuiLayer.h"
#include "debug/Instrumentor.h"
#include "gui/GUICore.h"
#include "physics/PhysicsContext.h"
#include "subsystem/SubSystem.h"
#include "debug/CrashHandler.h"

namespace BHive
{
	extern BHIVE_API Application *CreateApplication(const FCommandLine &commandline);

}

namespace BHive
{

	void SetImGuiContext(Application *app)
	{
#ifdef BUILD_SHARED
		ImGuiMemAllocFunc alloc_func = nullptr;
		ImGuiMemFreeFunc free_func = nullptr;
		void *user_data = nullptr;

		auto layer = app->GetImGuiLayer();
		auto ctx = layer->GetContext();

		layer->GetAllocatorCallbacks(&alloc_func, &free_func, &user_data);

		ImGui::SetCurrentContext((ImGuiContext *)ctx);
		ImGui::SetAllocatorFunctions(alloc_func, free_func, user_data);
#endif
	}

	int main(int argc, char **argv)
	{
		BHive::Log::Init();

		BH_PROFILE_BEGIN_SESSION("StartUp", "Profile-StartUp.json");
		auto app = BHive::CreateApplication({argc, argv});

		SetImGuiContext(app);

		BH_PROFILE_END_SESSION();

		auto &crash_handler = CrashHandler::Get();
		crash_handler.Init(app->GetSpecification().Title, "Crash_Report.log");

		try
		{
			BH_PROFILE_BEGIN_SESSION("Runtime", "Profile-Runtime.json");
			app->Run();
			BH_PROFILE_END_SESSION();
		}
		catch (const std::exception &e)
		{
			crash_handler.HandleException(e);

			return 1;
		}
		

		BH_PROFILE_BEGIN_SESSION("Shutdown", "Profile-Shutdown.json");
		delete app;
		BH_PROFILE_END_SESSION();

		return 0;
	}
} // namespace BHive

int main(int argc, char **argv)
{
	return BHive::main(argc, argv);
}