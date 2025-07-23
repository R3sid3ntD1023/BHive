#pragma once

#include "Application.h"
#include "audio/AudioContext.h"
#include "debug/Instrumentor.h"
#include <Windows.h>

namespace BHive
{
	extern BHIVE_API Application *CreateApplication(const FCommandLine &commandline);
}

namespace BHive
{
	int main(int argc, char **argv)
	{
		BHive::Log::Init();
		BHive::AudioContext::Init();
		BH_PROFILE_BEGIN_SESSION("StartUp", ENGINE_PATH "/../Profile-StartUp.json");
		auto app = BHive::CreateApplication({argc, argv});

		BH_PROFILE_END_SESSION();

		BH_PROFILE_BEGIN_SESSION("Runtime", ENGINE_PATH "/../Profile-Runtime.json");
		app->Run();
		BH_PROFILE_END_SESSION();

		BH_PROFILE_BEGIN_SESSION("Shutdown", ENGINE_PATH "/../Profile-Shutdown.json");
		delete app;
		BH_PROFILE_END_SESSION();

		BHive::AudioContext::Shutdown();

		return 0;
	}
} // namespace BHive

int main(int argc, char **argv)
{
	return BHive::main(argc, argv);
}