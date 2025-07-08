#pragma once

#include "Sprite.h"

namespace BHive
{
	struct Frame
	{
		Ref<Sprite> Sprite;

		uint32_t Duration{1};

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Sprite, Duration);
		}
	};

	class FlipBook : public Asset
	{
	public:
		typedef std::vector<Frame> Frames;

	public:
		FlipBook() = default;

		FlipBook(std::initializer_list<Frame> frames);

		void Play();
		void Stop();
		void Update(float deltatime);

		void SetLoop(bool loop);

		void SetFramesPerSecond(float fps);

		void AddFrame(const Ref<Sprite> &sprite, uint32_t duration = 1);
		void InsertFrame(const Ref<Sprite> &sprite, uint32_t duration = 1, uint32_t index = 0);

		Ref<Sprite> RemoveSprite(uint32_t index);

		void SetFrames(const Frames &frames);

		Ref<Sprite> GetCurrentSprite() const;

		const Frames &GetFrames() const;

		bool IsLooping() const { return mIsLooping; }

		bool IsPlaying() const { return mIsPlaying; }

		float GetFramesPerSecond() const { return mFramesPerSecond; }

		Ref<Sprite> GetSpriteAtFrame(int32_t frame) const;
		Ref<Sprite> GetSpriteAtTime(float time) const;

		float GetTotalTime() const;

		void Save(cereal::BinaryOutputArchive &ar) const;
		void Load(cereal::BinaryInputArchive &ar);

		REFLECTABLEV(Asset)

	private:
		int32_t GetNumFrames() const;

		int32_t GetFrameIndexAtTime(float time) const;

	private:
		Frames mFrames;

		float mFramesPerSecond = 15.0f;

		bool mIsLooping = false;

		bool mIsPlaying = false;

		float mCurrentTime = 0.0f;
	};

} // namespace BHive