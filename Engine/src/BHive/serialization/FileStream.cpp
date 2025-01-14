#include "FileStream.h"

namespace BHive
{
    FileStreamWriter::FileStreamWriter(const std::filesystem::path &path)
        : mPath(path)
    {
        mStream = std::ofstream(path, std::ios::out | std::ios::binary);
    }

    FileStreamWriter::~FileStreamWriter()
    {
        mStream.close();
    }

    bool FileStreamWriter::WriteImpl(const char *data, size_t size)
    {
        mStream.write(data, size);
        return true;
    }

    FileStreamReader::FileStreamReader(const std::filesystem::path &path)
        : mPath(path)
    {
        mStream = std::ifstream(path, std::ios::in | std::ios::binary);
    }

    FileStreamReader::~FileStreamReader()
    {
        mStream.close();
    }

    bool FileStreamReader::ReadImpl(char *data, size_t size)
    {
        mStream.read(data, size);
        return true;
    }

} // namespace BHive
