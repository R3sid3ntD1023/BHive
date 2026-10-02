#include "AudioSource.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

namespace BHive
{
	const char *GetOpenALErrorString(ALenum err)
	{
		switch (err)
		{
		case AL_NO_ERROR:
			return "AL_NO_ERROR";
		case AL_INVALID_NAME:
			return "AL_INVALID_NAME";
		case AL_INVALID_ENUM:
			return "AL_INVALID_ENUM";
		case AL_INVALID_VALUE:
			return "AL_INVALID_VALUE";
		case AL_INVALID_OPERATION:
			return "AL_INVALID_OPERATION";
		default:
			return "UNKNOWN_ERROR";
		}
	}

	AudioSource::AudioSource(const MemoryBlock<int16_t> &data, const FAudioSpecification &specs)
		: mSpecification(specs),
		  mLength((float)specs.mNumSamples / (float)specs.mSampleRate),
		  mBuffer(data)

	{
		Initialize();
	}

	AudioSource::~AudioSource()
	{
		alDeleteSources(1, &mSourceID);
		alDeleteBuffers(1, &mAudioID);
	}

	void AudioSource::Initialize()
	{
		alGenBuffers(1, &mAudioID);
		alBufferData(mAudioID, mSpecification.mFormat, mBuffer.GetData(), (ALsizei)mBuffer.GetSize(), mSpecification.mSampleRate);
		auto err = alGetError();
		ASSERT(err == AL_NO_ERROR, "{}", GetOpenALErrorString(err));

		bool hasLoopPoints = mSpecification.mStartLoop.has_value() && mSpecification.mEndLoop.has_value();
		if (hasLoopPoints)
		{
			ASSERT(alIsExtensionPresent("AL_SOFT_loop_points"));

			int startLoop = *mSpecification.mStartLoop;
			int endLoop = *mSpecification.mEndLoop;
			int offsets[2] = {startLoop, endLoop};
			alBufferiv(mAudioID, AL_LOOP_POINTS_SOFT, offsets);

			err = alGetError();
			ASSERT(err == AL_NO_ERROR, "{} - {} - {}", GetOpenALErrorString(err), offsets[0], offsets[1]);
		}

		alGenSources(1, &mSourceID);
		alSourcei(mSourceID, AL_BUFFER, mAudioID);
		err = alGetError();
		ASSERT(err == AL_NO_ERROR, "{}", GetOpenALErrorString(err));

		if (hasLoopPoints)
		{
			alSourcei(mSourceID, AL_LOOPING, AL_TRUE);
		}

		LOG_TRACE("length:{}, time:{}", GetLengthSeconds(), GetLength().to_string());
	}

	void AudioSource::Play()
	{
		int state = 0;
		alGetSourcei(mSourceID, AL_SOURCE_STATE, &state);
		if (state == AL_STOPPED)
		{
			mIsPlaying = false;
		}

		if (!mIsPlaying)
		{
			alSourcePlay(mSourceID);
			mIsPlaying = true;
		}
	}

	void AudioSource::Stop()
	{
		if (mIsPlaying)
		{
			alSourceStop(mSourceID);
			mIsPlaying = false;
		}
	}

	void AudioSource::Pause()
	{
		if (mIsPlaying)
		{
			alSourcePause(mSourceID);
			mIsPlaying = false;
		}
	}

	void AudioSource::SetLooping(bool loop)
	{
		mIsLooping = loop;
		alSourcei(mSourceID, AL_LOOPING, loop);
	}

	void AudioSource::SetPosition(float x, float y, float z)
	{
		mPosition[0] = x;
		mPosition[1] = y;
		mPosition[2] = z;
		alSource3f(mSourceID, AL_POSITION, x, y, z);
	}

	void AudioSource::SetPitch(float pitch)
	{
		mPitch = pitch;
		alSourcef(mSourceID, AL_PITCH, pitch);
	}

	void AudioSource::SetSpatial(bool spatial)
	{
		mIsSpatial = spatial;
		alSourcei(mSourceID, AL_SOURCE_SPATIALIZE_SOFT, spatial ? AL_TRUE : AL_FALSE);
		alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
	}

	void AudioSource::SetVolume(float gain)
	{
		mGain = gain;
		alSourcef(mSourceID, AL_GAIN, gain);
	}

	AudioTime AudioSource::GetPlaybackPosition() const
	{
		float seconds;
		alGetSourcef(mSourceID, AL_SEC_OFFSET, &seconds);
		return seconds;
	}

	void AudioSource::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);

		ar(mSpecification, mPitch, mGain, mIsLooping, mLength, mBuffer);
	}

	void AudioSource::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);

		ar(mSpecification, mPitch, mGain, mIsLooping, mLength, mBuffer);

		Initialize();
		SetPitch(mPitch);
		SetVolume(mGain);
		SetLooping(mIsLooping);
	}

	REFLECT(AudioSource)
	{
		BEGIN_REFLECT(AudioSource)
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY("Pitch", GetPitch, SetPitch)
		REFLECT_PROPERTY("Volume", GetVolume, SetVolume)
		REFLECT_PROPERTY("Loop", IsLooping, SetLooping)
		REFLECT_PROPERTY_READ_ONLY("Length", GetLength)
		REFLECT_PROPERTY_READ_ONLY("Length In Seconds", GetLengthSeconds);

		rttr::type::register_wrapper_converter_for_base_classes<Ref<AudioSource>>();
	}
} // namespace BHive
