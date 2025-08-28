#include "AudioContext.h"
#include <al/al.h>
#include <al/alc.h>
#include <al/alext.h>
#include <AL/efx.h>

namespace BHive
{

	void AudioContext::Init()
	{
		auto mDeviceName = alcGetString(0, ALC_DEFAULT_DEVICE_SPECIFIER);
		mAudioDevice = alcOpenDevice(mDeviceName);

		ASSERT(mAudioDevice);

		mAlContext = alcCreateContext((ALCdevice *)mAudioDevice, {0});
		ASSERT(mAlContext);
		ASSERT(alcMakeContextCurrent((ALCcontext *)mAlContext));

		ALfloat listenerPos[] = {0, 0, 0};
		ALfloat listenerVel[] = {0, 0, 0};
		ALfloat listenerOri[] = {0, 0, -1, 0, 1, 0};
		alListenerfv(AL_POSITION, listenerPos);
		alListenerfv(AL_VELOCITY, listenerVel);
		alListenerfv(AL_ORIENTATION, listenerOri);

		LOG_TRACE("Initialized Audio Context");

#ifdef _DEBUG
		PrintDeviceInfo();
#endif
	}

	void AudioContext::Shutdown()
	{
		alcMakeContextCurrent(NULL);
		alcDestroyContext((ALCcontext *)mAlContext);
		alcCloseDevice((ALCdevice *)mAudioDevice);

		LOG_TRACE("Shutdown OpenAL Audio");
	}

	void AudioContext::PrintDeviceInfo()
	{
		std::string name;
		int frequency = 0, sources_max = 0, num_mono_sources = 0, num_stereo_sources = 0;
		auto device = (ALCdevice *)mAudioDevice;

		alcGetIntegerv(device, ALC_FREQUENCY, 1, &frequency);
		alcGetIntegerv(device, ALC_MONO_SOURCES, 1, &num_mono_sources);
		alcGetIntegerv(device, ALC_STEREO_SOURCES, 1, &num_stereo_sources);
		name = alcGetString(device, ALC_DEFAULT_DEVICE_SPECIFIER);

#if _DEBUG
		LOG_INFO("Audio Device Info:");
		LOG_INFO("\tName: {}", name);
		LOG_INFO("\tSample Rate: {}", frequency);
		LOG_INFO("\t\tMono: {}", num_mono_sources);
		LOG_INFO("\t\tStereo: {}", num_stereo_sources);
#endif
	}
} // namespace BHive