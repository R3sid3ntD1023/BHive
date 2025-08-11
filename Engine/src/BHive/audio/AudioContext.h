#pragma once

#include "core/Core.h"

namespace BHive
{
	class AudioSource;

	class BHIVE_API AudioContext
	{
	public:
		void Init();
		void Shutdown();
		void PrintDeviceInfo();

	private:
		void *mAlContext = nullptr;
		void *mAudioDevice = nullptr;
	};
} // namespace BHive