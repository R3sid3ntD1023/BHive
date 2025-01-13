#pragma once

#include "core/Core.h"
#include "AudioTime.h"
#include "core/Buffer.h"
#include "AudioSpecification.h"
#include "asset/Asset.h"

namespace BHive
{

	class BHIVE AudioSource : public Asset
	{
	public:
		AudioSource() = default;
		AudioSource(int16_t *buffer, int size, const FAudioSpecification &specs = {});
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

		virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

		REFLECTABLEV(Asset)

	private:
		void Initialize();

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
		FAudioSpecification mSpecification;

		TBuffer<int16_t> mBuffer;
	};

	REFLECT_EXTERN(AudioSource)

} // namespace BHive