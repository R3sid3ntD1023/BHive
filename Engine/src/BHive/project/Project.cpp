#include "Project.h"
#include "serialization/FileStream.h"

namespace BHive
{
    void FProjectConfiguration::Deserialize(StreamReader& reader)
    {
        reader(mName);
        reader(mProjectDirectory);
        reader(mResourcesDirectory);
    }

    void FProjectConfiguration::Serialize(StreamWriter &writer) const
    {
        writer(mName);
        writer(mProjectDirectory);
        writer(mResourcesDirectory);
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

        FileStreamReader reader(path);
        if (reader)
        {
            reader(sActiveProject->mConfig);
        }

        return sActiveProject;
    }

    bool Project::Save(const std::filesystem::path &path)
    {
        ASSERT(sActiveProject);

        FileStreamWriter writer(path);
        if (!writer)
            return false;

        writer(sActiveProject->mConfig);

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