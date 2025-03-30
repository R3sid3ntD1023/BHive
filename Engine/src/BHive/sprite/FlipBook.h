#pragma once

#include "Sprite.h"

namespace BHive
{
	REFLECT_STRUCT()
	struct Frame
	{
		REFLECT_PROPERTY()
		Ref<Sprite> mSprite;

		REFLECT_PROPERTY()
		uint32_t mDuration{1};

		template <typename A>
		void Serialize(A &ar)
		{
			ar(mSprite, mDuration);
		}
	};

	REFLECT_CLASS()
	class FlipBook : public Asset
	{
	public:
		typedef std::vector<Frame> Frames;

	public:
		REFLECT_CONSTRUCTOR()
		FlipBook() = default;

		FlipBook(std::initializer_list<Frame> frames);

		void Play();
		void Stop();
		void Update(float deltatime);

		REFLECT_FUNCTION()
		void SetLoop(bool loop);

		REFLECT_FUNCTION()
		void SetFramesPerSecond(float fps);

		void AddFrame(const Ref<Sprite> &sprite, uint32_t duration = 1);
		void InsertFrame(const Ref<Sprite> &sprite, uint32_t duration = 1, uint32_t index = 0);

		Ref<Sprite> RemoveSprite(uint32_t index);

		REFLECT_FUNCTION()
		void SetFrames(const Frames &frames);

		Ref<Sprite> GetCurrentSprite() const;

		REFLECT_FUNCTION()
		const Frames &GetFrames() const;

		REFLECT_FUNCTION()
		bool IsLooping() const { return mIsLooping; }

		bool IsPlaying() const { return mIsPlaying; }

		REFLECT_FUNCTION()
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
		REFLECT_PROPERTY(Getter = GetFrames, Setter = SetFrames);
		Frames mFrames;

		REFLECT_PROPERTY(Getter = GetFramesPerSecond, Setter = SetFramesPerSecond)
		float mFramesPerSecond = 15.0f;

		REFLECT_PROPERTY(Getter = IsLooping, Setter = SetLoop)
		bool mIsLooping = false;

		bool mIsPlaying = false;

		float mCurrentTime = 0.0f;
	};

} // namespace BHive