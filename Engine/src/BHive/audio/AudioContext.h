#pragma once

#include "core/Core.h"

namespace BHive
{
	class AudioSource;

	class AudioContext
	{
	public:
		static BHIVE void Init();
		static BHIVE void Shutdown();
		static BHIVE void PrintDeviceInfo();
	};
}