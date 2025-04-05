#include "core/Application.h"
#include "core/EntryPoint.h"
#include "EditorLayer.h"

namespace BHive
{
	class BHiveEditor : public Application
	{
	public:
		BHiveEditor(const FApplicationSpecification &spec)
			: Application(spec)
		{
			PushLayer(new EditorLayer());
		}
	};

	Application *CreateApplication(const FCommandLine &cmd)
	{
		FApplicationSpecification spec{};
		spec.Title = "BHive Editor";
		spec.CommandLine = cmd;
		spec.Size = {1280, 720};
		spec.Maximize = true;
		return new BHiveEditor(spec);
	}
} // namespace BHive