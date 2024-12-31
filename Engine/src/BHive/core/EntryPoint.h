#pragma once

#include "Application.h"
#include "audio/AudioContext.h"
#include "debug/Instrumentor.h"

namespace BHive
{
	extern BHIVE Application *CreateApplication(const FCommandLine &commandline);
}

namespace BHive
{
	int main(int argc, char **argv)
	{

		Log::Init();
		AudioContext::Init();

		BH_PROFILE_BEGIN_SESSION("StartUp", ENGINE_PATH "/../info/Profile-StartUp.json");
		auto app = BHive::CreateApplication({argc, argv});
		BH_PROFILE_END_SESSION();

		BH_PROFILE_BEGIN_SESSION("Runtime", ENGINE_PATH "/../info/Profile-Runtime.json");
		app->Run();
		BH_PROFILE_END_SESSION();

		BH_PROFILE_BEGIN_SESSION("Shutdown", ENGINE_PATH "/../info/Profile-Shutdown.json");
		delete app;
		BH_PROFILE_END_SESSION();

		AudioContext::Shutdown();

		return 0;
	}
}

int main(int argc, char **argv)
{
	return BHive::main(argc, argv);
}