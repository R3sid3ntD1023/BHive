#include "AudioSource.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

namespace BHive
{
	AudioSource::AudioSource(int16_t *buffer, int size, const FAudioSpecification &specs)
		: mSpecification(specs),
		  mLength((float)specs.mNumSamples / (float)specs.mSampleRate)
	{
		mBuffer.Allocate(size);

		memcpy_s(mBuffer.mData, mBuffer.mSize, buffer, size);

		Initialize();
	}

	AudioSource::~AudioSource()
	{
		alDeleteSources(1, &mSourceID);
		alDeleteBuffers(1, &mAudioID);

		mBuffer.Release();
	}

	void AudioSource::Initialize()
	{
		alGenBuffers(1, &mAudioID);
		alBufferData(
			mAudioID, mSpecification.mFormat, mBuffer.mData, mBuffer.mSize,
			mSpecification.mSampleRate);

		if (mSpecification.mStartLoop.has_value() && mSpecification.mEndLoop.has_value())
		{
			int offsets[2] = {*mSpecification.mStartLoop, *mSpecification.mEndLoop};
			alBufferiv(mAudioID, AL_LOOP_POINTS_SOFT, offsets);
		}

		alGenSources(1, &mSourceID);
		alSourcei(mSourceID, AL_BUFFER, mAudioID);
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

	void AudioSource::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mSpecification, mPitch, mGain, mIsLooping, mLength);
		ar(mBuffer);
	}

	void AudioSource::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		ar(mSpecification, mPitch, mGain, mIsLooping, mLength);
		ar(mBuffer);

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
	}
} // namespace BHive
