#include "AudioContext.h"
#include <al/al.h>
#include <al/alc.h>
#include <al/alext.h>
#include <AL/efx.h>

namespace BHive
{
	auto alDebugMessageCallbackEXT = LPALDEBUGMESSAGECALLBACKEXT{};
	auto alDebugMessageControlEXT = LPALDEBUGMESSAGECONTROLEXT{};

	void AudioContext::Init()
	{
		auto mDeviceName = alcGetString(0, ALC_DEFAULT_DEVICE_SPECIFIER);
		mAudioDevice = alcOpenDevice(mDeviceName);

		ASSERT(mAudioDevice);

		mAlContext = alcCreateContext((ALCdevice *)mAudioDevice, {0});
		ASSERT(mAlContext);
		ASSERT(alcMakeContextCurrent((ALCcontext *)mAlContext));

#if _DEBUG
		auto debug_supported = alcIsExtensionPresent((ALCdevice *)mAudioDevice, "ALC_EXT_debug");
		if (!debug_supported)
		{
			LOG_WARN("alc debugging not supported");
		}

	#define LOAD_AL_FUNC(N) N = reinterpret_cast<decltype(N)>(alcGetProcAddress((ALCdevice *)mAudioDevice, #N))

		LOAD_AL_FUNC(alDebugMessageControlEXT);
		LOAD_AL_FUNC(alDebugMessageCallbackEXT);

	#undef LOAD_AL_FUNC

		static constexpr auto al_debug_callback = [](ALenum source, ALenum type, ALuint id, ALenum severity, ALsizei length, const ALchar *message, void *userParam) noexcept -> void
		{
			// Ignore non-significant error/warning codes
			/*if (severity == AL_DEBUG_SEVERITY_NOTIFICATION_EXT)
				return;*/
			switch (severity)
			{
			case AL_DEBUG_SEVERITY_HIGH_EXT:
				LOG_ERROR("OpenAL Debug: {}", message);
				break;
			case AL_DEBUG_SEVERITY_MEDIUM_EXT:
				LOG_WARN("OpenAL Debug: {}", message);
				break;
			case AL_DEBUG_SEVERITY_LOW_EXT:
				LOG_INFO("OpenAL Debug: {}", message);
				break;
			default:
				LOG_TRACE("OpenAL Debug: {}", message);
				break;
			}
		};

		// Enable all debug messages
		alDebugMessageControlEXT(AL_DONT_CARE_EXT, AL_DONT_CARE_EXT, AL_DEBUG_SEVERITY_LOW_EXT, 0, nullptr, AL_TRUE);
		alDebugMessageCallbackEXT(al_debug_callback, nullptr);

#endif

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

#if _DEBUG
		alDebugMessageCallbackEXT(nullptr, nullptr);
#endif //  _DEBUG

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