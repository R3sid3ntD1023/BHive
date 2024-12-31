#include "AudioContext.h"
#include <al/al.h>
#include <al/alc.h>
#include <al/alext.h>

namespace BHive
{
	static ALCcontext *sAlContext = nullptr;
	static ALCdevice *sAudioDevice = nullptr;

	void AudioContext::Init()
	{
		auto mDeviceName = alcGetString(0, ALC_DEFAULT_DEVICE_SPECIFIER);
		sAudioDevice = alcOpenDevice(mDeviceName);

		int attributes[] = {0};
		sAlContext = alcCreateContext(sAudioDevice, attributes);

		alcMakeContextCurrent(sAlContext);

		PrintDeviceInfo();

		ALfloat listenerPos[] = {0, 0, 0};
		ALfloat listenerVel[] = {0, 0, 0};
		ALfloat listenerOri[] = {0, 0, -1, 0, 1, 0};
		alListenerfv(AL_POSITION, listenerPos);
		alListenerfv(AL_VELOCITY, listenerVel);
		alListenerfv(AL_ORIENTATION, listenerOri);

		LOG_TRACE("Initialized Audio Context");
	}

	void AudioContext::Shutdown()
	{
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(sAlContext);
		alcCloseDevice(sAudioDevice);

		LOG_TRACE("Closed Audio Context");
	}

	void AudioContext::PrintDeviceInfo()
	{
		std::string name;
		int frequency = 0, sources_max = 0, num_mono_sources = 0, num_stereo_sources = 0;

		alcGetIntegerv(sAudioDevice, ALC_FREQUENCY, 1, &frequency);
		alcGetIntegerv(sAudioDevice, ALC_MONO_SOURCES, 1, &num_mono_sources);
		alcGetIntegerv(sAudioDevice, ALC_STEREO_SOURCES, 1, &num_stereo_sources);
		name = alcGetString(sAudioDevice, ALC_DEFAULT_DEVICE_SPECIFIER);

#if _DEBUG
		LOG_INFO("Audio Device Info:");
		LOG_INFO("\tName: {}", name);
		LOG_INFO("\tSample Rate: {}", frequency);
		LOG_INFO("\t\tMono: {}", num_mono_sources);
		LOG_INFO("\t\tStereo: {}", num_stereo_sources);
#endif
	}
}