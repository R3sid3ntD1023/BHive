#include "AudioSource.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

namespace BHive
{
	AudioSource::AudioSource(int format, int16_t *buffer, int size, int sampleRate, float length, const AudioSpecification &specs)
		: mSpecification(specs),
		  mData(buffer),
		  mSize(size),
		  mFormat(format)
	{
		mLength = length;

		alGenBuffers(1, &mAudioID);
		alBufferData(mAudioID, format, buffer, size, sampleRate);

		if (mSpecification.StartLoop.has_value() && mSpecification.EndLoop.has_value())
		{
			int offsets[2] = {*mSpecification.StartLoop, *mSpecification.EndLoop};
			alBufferiv(mAudioID, AL_LOOP_POINTS_SOFT, offsets);
		}

		alGenSources(1, &mSourceID);
		alSourcei(mSourceID, AL_BUFFER, mAudioID);
	}

	AudioSource::~AudioSource()
	{
		alDeleteSources(1, &mSourceID);
		alDeleteBuffers(1, &mAudioID);
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

	void AudioSource::Serialize(StreamWriter &ar) const
	{
		ar(mPitch, mGain, mIsLooping, mFormat, mLength, mSpecification, BinaryData(mData, mSize));
	}

	void AudioSource::Deserialize(StreamReader &ar)
	{
		ar(mPitch, mGain, mIsLooping, mFormat, mLength, mSpecification, BinaryData(mData, mSize));

		SetPitch(mPitch);
		SetVolume(mGain);
		SetLooping(mIsLooping);
	}

	REFLECT(AudioSource)
	{
		BEGIN_REFLECT(AudioSource)
		REFLECT_PROPERTY("Pitch", GetPitch, SetPitch)
		REFLECT_PROPERTY("Volume", GetVolume, SetVolume)
		REFLECT_PROPERTY("Loop", IsLooping, SetLooping)
		REFLECT_PROPERTY_READ_ONLY("Length", GetLength)
		REFLECT_PROPERTY_READ_ONLY("Length In Seconds", GetLengthSeconds);
	}
}
