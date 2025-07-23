#pragma once

#include "core/Core.h"

namespace BHive
{
	class AudioSource;

	class BHIVE_API AudioContext
	{
	public:
		static void Init();
		static void Shutdown();
		static void PrintDeviceInfo();
	};
} // namespace BHive