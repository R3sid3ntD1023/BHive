#pragma once

#include "core/Core.h"
#include "AudioTime.h"
#include "asset/Asset.h"
#include "serialization/Serialization.h"

namespace BHive
{

	struct AudioSpecification
	{
		std::optional<int> StartLoop;
		std::optional<int> EndLoop;
	};

	class BHIVE AudioSource : public Asset
	{
	public:
		AudioSource() = default;
		AudioSource(int format, int16_t *buffer, int size, int sampleRate, float length, const AudioSpecification &specs = {});
		~AudioSource();

		void Play();
		void Stop();
		void Pause();
		void SetLooping(bool loop);
		void SetPosition(float x, float y, float z);
		void SetPitch(float pitch);
		void SetSpatial(bool spatial);
		void SetVolume(float gain);

		bool IsPlaying() const { return mIsPlaying; }
		float GetLengthSeconds() const { return mLength; }
		AudioTime GetLength() const { return mLength; }
		float GetVolume() const { return mGain; }
		float GetPitch() const { return mPitch; }
		bool IsLooping() const { return mIsLooping; }

		void Serialize(StreamWriter &ar) const;
		void Deserialize(StreamReader &ar);

		ASSET_CLASS(AudioSource)
		REFLECTABLEV(Asset)

	private:
		uint32_t mAudioID{0};
		uint32_t mSourceID{0};
		bool mIsPlaying{false};

		float mPosition[3] = {0, 0, 0};
		float mGain = 1.0f;
		float mPitch = 1.0f;
		bool mIsSpatial{false};
		bool mIsLooping{false};

		float mLength{0.0f};
		AudioSpecification mSpecification;

		int16_t *mData = nullptr;
		size_t mSize = 0;
		unsigned mFormat;
	};

	REFLECT_EXTERN(AudioSource)
}