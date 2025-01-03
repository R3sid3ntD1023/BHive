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

    template <typename T1, typename T2>
    inline void Serialize(StreamWriter &ar, const std::pair<T1, T2> &obj)
    {
        ar(obj.first, obj.second);
    }

    template <typename T, typename Traits, typename Alloc>
    inline void Serialize(StreamWriter &writer, const std::basic_string<T, Traits, Alloc> &str)
    {
        writer(str.size());
        writer.WriteImpl(str.data(), sizeof(T) * str.size());
    }

    inline void Serialize(StreamWriter &writer, const std::filesystem::path &path)
    {
        writer(path.string());
    }

    template <typename T>
    inline void Serialize(StreamWriter &writer, const std::vector<T> &obj)
    {
        writer(obj.size());

        for (auto &v : obj)
            writer(v);
    }

     template <template <typename...> class Map, typename... TArgs, typename = typename Map<TArgs...>::mapped_type>
    inline void Serialize(StreamWriter &writer, const Map<TArgs...> &map)
    {
        writer(map.size());

        for (const auto &i : map)
        {
            writer(i.first, i.second);
        }
    }

   

} // namespace BHive
