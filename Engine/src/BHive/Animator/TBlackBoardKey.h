#pragma once

#include "core/Core.h"
#include "BlackBoardKey.h"

namespace BHive
{
    template<typename T>
    class TBlackBoardKey : public BlackBoardKey
    {
    private:
        /* data */
    public:
        TBlackBoardKey(/* args */) = default;
        TBlackBoardKey( const T& initial_value)
            :mValue(initial_value)
        {}

        ~TBlackBoardKey() = default;

        virtual std::any Get() const
        {
            return mValue;
        };

        virtual void Set(const std::any& value)
        {
            mValue = std::any_cast<T>(value);
        }

        virtual bool Compare(const std::any& value) const 
        {
            return value.type() == typeid(T) && mValue == std::any_cast<T>(value);
        }

    private:
        T mValue{};
    };
    
} // namespace BHive
