#include "BHive.h"
#include "project/Project.h"

namespace BHive
{
    // stores launcher settings
    struct FProjectLauncherSettings
    {
        std::unordered_map<std::string, std::filesystem::path> mRecentProjectPaths;


        template<typename A>
        void SERIALIZE(A& ar)
        {
			ar(MAKE_NVP("RecentProjects", mRecentProjectPaths));
        }
    };

    class ProjectLauncherLayer : public Layer
    {
        void OnAttach() override;

        void OnDetach() override;

        void OnGuiRender(float) override;

    private:
        void OpenProject(const std::filesystem::path &path);
        void CreateProject(const FProjectConfiguration &config, std::string& message);

    private:
        FProjectLauncherSettings mSettings;
		std::string mMessage;
    };

} // namespace BHive
