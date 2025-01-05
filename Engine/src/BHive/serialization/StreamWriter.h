#pragma once

#include <stdint.h>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <map>
#include "SerializeTraits.h"

namespace BHive
{
    class StreamWriter
    {
    public:
        virtual ~StreamWriter() = default;

        template <typename... TArgs>
        StreamWriter &operator()(TArgs &&...args)
        {
            Process(std::forward<TArgs>(args)...);
            return *this;
        }

        virtual bool IsGood() const = 0;
        virtual uint64_t GetStreamPosition() = 0;
        virtual void SetStreamPosition(uint64_t position) = 0;
        virtual bool WriteImpl(const char *data, size_t size) = 0;

        operator bool() const { return IsGood(); }

        template <typename TArg>
        inline void Process(TArg &&arg)
        {

            Write(arg);
        }

        template <typename TArg, typename... TArgs>
        inline void Process(TArg &&arg, TArgs &&...args)
        {
            Process(std::forward<TArg>(arg));
            Process(std::forward<TArgs>(args)...);
        }

        template <typename T, std::enable_if_t<has_serialize_v<StreamWriter, T>, bool> = true>
        inline void Write(const T &obj)
        {
            accessor::Serialize(*this, obj);
        }

        template <typename T, std::enable_if_t<has_serialize_non_member_v<StreamWriter, T>, bool> = true>
        inline void Write(const T &obj)
        {
            Serialize(*this, obj);
        }

        template <typename T, std::enable_if_t<!is_serializable_v<StreamWriter, T>, bool> = true>
        inline void Write(const T &type)
        {
            WriteImpl((char *)&type, sizeof(T));
        }
    };
} // namespace BHive
