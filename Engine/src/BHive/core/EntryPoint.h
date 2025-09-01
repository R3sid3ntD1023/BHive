#pragma once

#include "Application.h"
#include "audio/AudioContext.h"
#include "debug/Instrumentor.h"
#include "physics/PhysicsContext.h"
#include "subsystem/SubSystem.h"

namespace BHive
{
	extern BHIVE_API Application *CreateApplication(const FCommandLine &commandline);
}

namespace BHive
{

	int main(int argc, char **argv)
	{
		BHive::Log::Init();

		AddSubSystem<AudioContext>().Init();
		AddSubSystem<PhysicsContext>().Init();

		BH_PROFILE_BEGIN_SESSION("StartUp", "Profile-StartUp.json");
		auto app = BHive::CreateApplication({argc, argv});

		BH_PROFILE_END_SESSION();

		BH_PROFILE_BEGIN_SESSION("Runtime", "Profile-Runtime.json");
		app->Run();
		BH_PROFILE_END_SESSION();

		BH_PROFILE_BEGIN_SESSION("Shutdown", "Profile-Shutdown.json");
		delete app;
		BH_PROFILE_END_SESSION();

		GetSubSystem<PhysicsContext>().Shutdown();
		GetSubSystem<AudioContext>().Shutdown();

		return 0;
	}
} // namespace BHive

int main(int argc, char **argv)
{
	return BHive::main(argc, argv);
}