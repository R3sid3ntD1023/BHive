#pragma once

#include "core/Core.h"

namespace BHive
{
    class SubSystemContext
    {
    public:
        static SubSystemContext &Get()
        {
            static SubSystemContext context;
            return context;
        }

        template <typename S, typename... TArgs>
        S &AddSubSystem(TArgs &&...args)
        {
            auto hash_code = GetSystemHashCode<S>();
            mSubSystems.emplace(hash_code, CreateRef<S>(std::forward<TArgs>(args)...));
            return GetSubSystem<S>();
        }

        template <typename S>
        S &GetSubSystem()
        {
            ASSERT(HasSubSystems<S>(), "Failed to find subsystem!");

            auto hash_code = GetSystemHashCode<S>();
            return *reinterpret_cast<S *>(mSubSystems.at(hash_code).get());
        }

        template <typename S>
        void RemoveSubSystem()
        {
            ASSERT(HasSubSystems<S>(), "Failed to find subsystem!");

            auto hash_code = GetSystemHashCode<S>();
            mSubSystems.erase(hash_code);
        }

        template <typename S>
        bool HasSubSystems() const
        {
            auto hash_code = GetSystemHashCode<S>();
            return mSubSystems.contains(hash_code);
        }

        template <typename S1, typename S2, typename... TArgs>
        bool HasSubSystems() const
        {
            return HasSubSystems<S1>() && HasSubSystems<S2, TArgs...>();
        }

    protected:
        template <typename S>
        size_t GetSystemHashCode() const
        {
            return typeid(S).hash_code();
        }

    protected:
        std::unordered_map<size_t, Ref<void>> mSubSystems;
    };
} // namespace BHive
