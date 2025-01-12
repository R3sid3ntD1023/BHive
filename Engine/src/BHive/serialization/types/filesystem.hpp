#pragma once


#include <filesystem>

namespace std::filesystem
{
    template<typename TArchive>
    inline void Serialize(TArchive& ar, const path& path)
    {
        ar(path.string());
    }

    template <typename TArchive>
	inline void Deserialize(TArchive &ar, path &path)
	{
		string value;
		ar(value);
		path = value;
	}

}