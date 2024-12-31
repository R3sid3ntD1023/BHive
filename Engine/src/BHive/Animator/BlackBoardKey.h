#pragma once

#include "core/Core.h"
#include <any>

namespace BHive
{
    class BlackBoardKey
    {
    private:
        /* data */
    public:
        virtual ~BlackBoardKey() = default;

        virtual std::any Get() const = 0;

        virtual void Set(const std::any& value) = 0;

        virtual bool Compare(const std::any& value) const = 0;

        template<typename T>
        T GetAs() const
        {
            return std::any_cast<T>(Get()); 
        }
    };
} // namespace BHive
