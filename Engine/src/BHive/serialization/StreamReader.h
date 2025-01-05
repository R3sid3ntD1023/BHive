#pragma once

#include "SerializeTraits.h"
#include <filesystem>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <map>

namespace BHive
{
    class StreamReader
    {
    public:
        virtual bool IsGood() const = 0;
        virtual uint64_t GetStreamPosition() = 0;
        virtual void SetStreamPosition(uint64_t position) = 0;
        virtual bool ReadImpl(char *data, size_t size) = 0;
        

        operator bool() const { return IsGood(); }

        template <typename... TArgs>
        StreamReader &operator()(TArgs &&...args)
        {
            Process(std::forward<TArgs>(args)...);
            return *this;
        }

        template <typename TArg>
        void Process(TArg &&arg)
        {
            Read(arg);
        }

        template <typename TArg, typename... TArgs>
        void Process(TArg &&arg, TArgs &&...args)
        {
            Process(std::forward<TArg>(arg));
            Process(std::forward<TArgs>(args)...);
        }

        template <typename T, std::enable_if_t<has_deserialize_v<StreamReader, T>, bool> = true>
        void Read(T &obj)
        {
            accessor::Deserialize(*this, obj);
        }

        template <typename T, std::enable_if_t<has_deserialize_non_member_v<StreamReader, T>, bool> = true>
        void Read(T &obj)
        {
            Deserialize(*this, obj);
        }

        template <typename T, std::enable_if_t<!is_deserializable_v<StreamReader, T>, bool> = true>
        void Read(T &type)
        {
            ReadImpl((char *)&type, sizeof(T));
        }
    };

} // namespace BHive
