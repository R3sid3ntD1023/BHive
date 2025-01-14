#include "Project.h"
#include "serialization/FileStream.h"

namespace BHive
{
    void FProjectConfiguration::Deserialize(StreamReader& ar)
    {
        ar(mName);
        ar(mProjectDirectory);
        ar(mResourcesDirectory);
    }

    void FProjectConfiguration::Serialize(StreamWriter &ar) const
    {
        ar(mName);
        ar(mProjectDirectory);
        ar(mResourcesDirectory);
    }

    Project::Project(const FProjectConfiguration &config)
        : mConfig(config)
    {
       
    }

    Ref<Project> Project::New(const FProjectConfiguration &config)
    {
        sActiveProject = CreateRef<Project>(config);
        return sActiveProject;
    }

    Ref<Project> Project::Load(const std::filesystem::path &path)
    {
        sActiveProject = CreateRef<Project>();

        FileStreamReader ar(path);
        if (ar)
        {
            ar(sActiveProject->mConfig);
        }

        return sActiveProject;
    }

    bool Project::Save(const std::filesystem::path &path)
    {
        ASSERT(sActiveProject);

        FileStreamWriter ar(path);
        if (!ar)
            return false;

        ar(sActiveProject->mConfig);

        return true;
    }

    REFLECT(FProjectConfiguration)
    {
        BEGIN_REFLECT(FProjectConfiguration)
        REFLECT_PROPERTY("Name", mName)
        REFLECT_PROPERTY("ProjectDirectory", mProjectDirectory)
        REFLECT_PROPERTY("ResourcesDirectory", mResourcesDirectory);
    }
}