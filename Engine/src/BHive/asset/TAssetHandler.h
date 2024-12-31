#pragma once

#include "core/Core.h"
#include "serialization/Serialization.h"
#include "reflection/Reflection.h"
#include "AssetManager.h"

namespace BHive
{

    struct AssetHandleBase
    {
        virtual Ref<Asset> get_asset() const = 0;

        virtual void set_asset(const Ref<Asset> &asset) = 0;

        virtual void set_asset(std::nullptr_t) = 0;

        REFLECTABLEV()
    };

    template <typename T>
    struct TAssetHandle : public AssetHandleBase
    {
        constexpr TAssetHandle() noexcept = default;

        constexpr TAssetHandle(std::nullptr_t) noexcept {}

        template <typename U, std::enable_if_t<std::is_base_of_v<T, U> || std::is_same_v<T, U>, bool> = true>
        TAssetHandle(const Ref<U> &ptr) : mPtr(ptr) {}

        TAssetHandle(const TAssetHandle &rhs) : mPtr(rhs.mPtr) {}

        Ref<T> get() const { return mPtr; }

        T *operator->() const { return mPtr.get(); }

        T &operator*() const { return *mPtr; }

        virtual Ref<Asset> get_asset() const { return std::dynamic_pointer_cast<Asset>(mPtr); }

        void set_asset(const Ref<Asset> &asset)
        {
            if (auto ptr = Cast<T>(asset))
            {
                mPtr = ptr;
            }
        }

        void set_asset(std::nullptr_t) { mPtr = nullptr; }

        template <typename U, std::enable_if_t<std::is_base_of_v<T, U> || std::is_same_v<T, U>, bool> = true>
        TAssetHandle &operator=(const Ref<U> &ptr)
        {
            mPtr = ptr;
            return *this;
        }

        TAssetHandle &operator=(const TAssetHandle &rhs)
        {
            mPtr = rhs.mPtr;
            return *this;
        }

        operator bool() const { return mPtr != nullptr; }
        operator Ref<T>() const { return mPtr; }

        REFLECTABLEV(AssetHandleBase)

    private:
        Ref<T> mPtr;
    };

    template <typename T>
    void Serialize(StreamWriter &ar, const TAssetHandle<T> &obj)
    {
        auto handle = obj ? obj.get()->Handle : AssetHandle(0);
        ar(handle);
    }

    template <typename T>
    void Deserialize(StreamReader &ar, TAssetHandle<T> &obj)
    {
        AssetHandle handle = 0;
        ar(handle);

        obj = AssetManager::GetAsset<T>(handle);
    }
}

namespace rttr
{
    template <typename T>
    struct wrapper_mapper<BHive::TAssetHandle<T>>
    {
        using wrapped_type = std::shared_ptr<T>;
        using type = BHive::TAssetHandle<T>;

        inline static wrapped_type get(const type &obj)
        {
            return obj.get();
        }

        inline static type create(const wrapped_type &value)
        {
            return BHive::TAssetHandle<T>(value);
        }

        template <typename U>
        inline static BHive::TAssetHandle<U> convert(const type &source, bool &ok)
        {
            if (auto obj = rttr_cast<typename BHive::TAssetHandle<U>::wrapped_type *>(source.get_asset()))
            {
                ok = true;
                return BHive::TAssetHandle<U>(*obj);
            }
            else
            {
                ok = false;
                return BHive::TAssetHandle<U>();
            }
        }
    };
}