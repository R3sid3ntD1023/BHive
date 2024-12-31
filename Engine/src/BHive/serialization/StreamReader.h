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

    template <typename T1, typename T2>
    inline void Deserialize(StreamReader &ar, std::pair<T1, T2> &obj)
    {
        ar(obj.first, obj.second);
    }

    template <typename T, typename Traits, typename Alloc>
    inline void Deserialize(StreamReader &reader, std::basic_string<T, Traits, Alloc> &str)
    {
        size_t size;
        reader(size);

        str.resize(size);
        reader.ReadImpl(str.data(), sizeof(T) * size);
    }

    inline void Deserialize(StreamReader &reader, std::filesystem::path &path)
    {
        std::string path_str;
        reader(path_str);
        path = path_str;
    }

    template <typename T>
    inline void Deserialize(StreamReader &reader, std::vector<T> &obj)
    {
        size_t size = 0;
        reader(size);
        obj.clear();

        obj.resize(size);

        for (size_t i = 0; i < size; i++)
        {
            reader(obj[i]);
        }
    }

    template <template <typename...> class Map, typename... TArgs, typename = typename Map<TArgs...>::mapped_type>
    void Deserialize(StreamReader &reader, Map<TArgs...> &map)
    {
        size_t size;

        reader(size);

        map.clear();
        auto hint = map.begin();

        for (size_t i = 0; i < size; i++)
        {
            typename Map<TArgs...>::key_type key;
            typename Map<TArgs...>::mapped_type value;

            reader(key, value);

            hint = map.emplace_hint(hint, std::move(key), std::move(value));
        }
    }

} // namespace BHive
