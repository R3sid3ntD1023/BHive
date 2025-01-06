#include "InputContextFactory.h"
#include "input/InputContext.h"
#include "asset/AssetSerializer.h"

namespace BHive
{
    void InputContextFactory::CreateNew(const std::filesystem::path &path)
    {

        InputContext context;
        AssetSerializer::serialize(context, path);
    }

    bool InputContextFactory::Import(Ref<Asset> &asset, const std::filesystem::path &path)
    {
        auto _asset = CreateRef<InputContext>();
        if (AssetSerializer::deserialize(*_asset, path))
        {
            asset = _asset;
            return true;
        }

        return false;
    }

    REFLECT_Factory(InputContextFactory, InputContext, ".input")

} // namespace BHive
