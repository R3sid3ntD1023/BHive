#pragma once

#include "StreamReader.h"
#include "StreamWriter.h"
#include <fstream>

namespace BHive
{
	class IniWriter : public StreamWriter
	{
	public:
		IniWriter(const std::filesystem::path &path);
		IniWriter(const IniWriter &) = delete;
		virtual ~IniWriter();

		virtual bool IsGood() const final { return mStream.good(); };
		virtual uint64_t GetStreamPosition() final { return mStream.tellp(); };
		virtual void SetStreamPosition(uint64_t position) final { mStream.seekp(position); };
		virtual bool WriteImpl(const char *data, size_t size) final;

	private:
		std::filesystem::path mPath;
		std::ofstream mStream;
	};

	class IniReader : public StreamReader
	{
	public:
		IniReader(const std::filesystem::path &path);
		IniReader(const IniReader &) = delete;
		virtual ~IniReader();

		virtual bool IsGood() const final { return mStream.good(); };
		virtual uint64_t GetStreamPosition() final { return mStream.tellg(); };
		virtual void SetStreamPosition(uint64_t position) final { mStream.seekg(position); };
		virtual bool ReadImpl(char *data, size_t size);

	private:
		std::filesystem::path mPath;
		std::ifstream mStream;
	};
}  // namespace BHive