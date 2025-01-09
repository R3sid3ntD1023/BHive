#pragma once

#include "StreamReader.h"
#include "StreamWriter.h"
#include "Helpers.h"
#include <fstream>


namespace BHive
{
    class FileStreamWriter : public StreamWriter
    {
    public:
        FileStreamWriter(const std::filesystem::path &path);
        FileStreamWriter(const FileStreamWriter &) = delete;
        virtual ~FileStreamWriter();

        virtual bool IsGood() const final { return mStream.good(); };
        virtual uint64_t GetStreamPosition() final { return mStream.tellp(); };
        virtual void SetStreamPosition(uint64_t position) final { mStream.seekp(position); };
        virtual bool WriteImpl(const char *data, size_t size) final;

    private:
        std::filesystem::path mPath;
        std::ofstream mStream;
    };

    class FileStreamReader : public StreamReader
    {
    public:
        FileStreamReader(const std::filesystem::path &path);
        FileStreamReader(const FileStreamReader &) = delete;
        virtual ~FileStreamReader();

        virtual bool IsGood() const final { return mStream.good(); };
        virtual uint64_t GetStreamPosition() final { return mStream.tellg(); };
        virtual void SetStreamPosition(uint64_t position) final { mStream.seekg(position); };
        virtual bool ReadImpl(char *data, size_t size);

    private:
        std::filesystem::path mPath;
        std::ifstream mStream;
    };

    template<typename T>
    void Serialize(FileStreamWriter& ar, const BinaryData<T>& data)
    {
		ar.WriteImpl(data.mData, data.mSize);
    }

    template <typename T>
	void Deserialize(FileStreamReader &ar, BinaryData<T> &data)
	{
		ar.ReadImpl(data.mData, data.mSize);
	}
} // namespace BHive
