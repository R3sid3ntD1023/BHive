#pragma once

#include "Sprite.h"

namespace BHive
{
	typedef TAssetHandle<Sprite> sprite_ptr;

	class FlipBook : public Asset
	{
	public:
		struct Frame
		{
			sprite_ptr mSprite;
			uint32_t mDuration{1};
		};

		typedef std::vector<FlipBook::Frame> Frames;

	public:
		FlipBook() = default;
		FlipBook(std::initializer_list<Frame> frames);

		void Play();
		void Stop();
		void Update(float deltatime);
		void SetLoop(bool loop);
		void SetFramesPerSecond(float fps);

		void AddSprite(const sprite_ptr &sprite, uint32_t duration = 1, uint32_t index = 0);
		sprite_ptr RemoveSprite(uint32_t index);

		const Frames &GetFrames() const;
		void SetFrames(const Frames &frames);

		sprite_ptr GetCurrentSprite() const;

		bool IsLooping() const { return mIsLooping; }
		bool IsPlaying() const { return mIsPlaying; }
		float GetFramesPerSecond() const { return mFramesPerSecond; }

		sprite_ptr GetSpriteAtFrame(int32_t frame) const;
		sprite_ptr GetSpriteAtTime(float time) const;

		float GetTotalTime() const;

		REFLECTABLEV(Asset)

		virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

	private:
		int32_t GetNumFrames() const;

		int32_t GetFrameIndexAtTime(float time) const;

	private:
		Frames mFrames;
		float mFramesPerSecond = 15.0f;
		bool mIsPlaying = false;
		bool mIsLooping = false;
		float mCurrentTime = 0.0f;
	};

	template <typename A>
	void Serialize(A &ar, FlipBook::Frame &obj)
	{
		ar(MAKE_NVP("Sprite", obj.mSprite), MAKE_NVP("Duration", obj.mDuration));
	}

	REFLECT_EXTERN(FlipBook::Frame)
	REFLECT_EXTERN(FlipBook)

} // namespace BHive