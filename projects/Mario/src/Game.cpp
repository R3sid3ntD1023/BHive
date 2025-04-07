#include "core/EntryPoint.h"
#include "GameLayer.h"

namespace BHive
{
	class Game : public Application
	{
	public:
		Game(const FApplicationSpecification &specs)
			: Application(specs)
		{
			// PushLayer(new GameLayer());
		}
	};

	Application *CreateApplication(const FCommandLine &cmd)
	{
		FApplicationSpecification specs{};
		specs.Title = "Mario";
		specs.CommandLine = cmd;
		specs.Size = {800, 600};
		specs.Maximize = true;
		return new Game(specs);
	}
} // namespace BHive