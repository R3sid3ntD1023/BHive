#include "BHive.h"
#include "core/EntryPoint.h"
#include "EditorLayer.h"

namespace BHive
{
    class EditorApplication : public Application
    {
    public:
        EditorApplication(const FApplicationSpecification &specification)
            : Application(specification)
        {
            mEditorLayer = new EditorLayer();
            PushLayer(mEditorLayer);
        }

    private:
        EditorLayer *mEditorLayer = nullptr;
    };

    Application *CreateApplication(const FCommandLine &cmdline)
    {
        FApplicationSpecification specification;
        specification.CommandLine = cmdline;
        specification.Title = "Editor v" + GET_ENGINE_VERSION.to_string();
        specification.Width = 800;
        specification.Height = 600;
        specification.VSync = false;
        return new EditorApplication(specification);
    }
}