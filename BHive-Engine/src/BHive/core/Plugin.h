#pragma once

#include "Core.h"

namespace BHive
{
	class Plugin
	{
	public:
		static Plugin Load(const std::string &path)
		{
			Plugin p;
			p.mHandle = LoadLibrary(path.c_str());
			return p;
		}

		template <typename Fn>
		Fn GetFunction(const char *name)
		{
			return reinterpret_cast<Fn>(GetProcAddress(mHandle, name));
		}

		~Plugin()
		{
			if (mHandle)
				FreeLibrary(mHandle);
		}

	private:
		HMODULE mHandle = nullptr;
	};
} // namespace BHive