#pragma once

#include "asset/Asset.h"
#include "gfx/registries/Handles.h"

namespace BHive
{

	class BHIVE_API FlipBook : public Asset
	{
	public:
		struct Frame
		{
			SpritePtr Sprite;

			uint32_t Duration{1};

			template <typename A>
			void Serialize(A &ar)
			{
				ar(Sprite, Duration);
			}
		};

	public:
		FlipBook() = default;

		FlipBook(const std::vector<Frame> &frames);

		void Play();

		void Stop();

		void Update(float deltatime);

		void SetLoop(bool loop);

		void SetFramesPerSecond(float fps);

		void AddFrame(SpritePtr sprite, uint32_t duration = 1);

		void InsertFrame(SpritePtr sprite, uint32_t duration = 1, uint32_t index = 0);

		SpritePtr RemoveSprite(uint32_t index);

		void SetFrames(const std::vector<FlipBook::Frame> &frames);

		SpritePtr GetCurrentSprite() const;

		const auto &GetFrames() const { return mFrames; }

		bool IsLooping() const { return mIsLooping; }

		bool IsPlaying() const { return mIsPlaying; }

		float GetFramesPerSecond() const { return mFramesPerSecond; }

		SpritePtr GetSpriteAtFrame(int32_t frame) const;

		SpritePtr GetSpriteAtTime(float time) const;

		float GetTotalTime() const;

		void Save(cereal::BinaryOutputArchive &ar) const;
		void Load(cereal::BinaryInputArchive &ar);

		REFLECTABLEV(Asset)

	private:
		int32_t GetNumFrames() const;

		int32_t GetFrameIndexAtTime(float time) const;

	private:
		std::vector<Frame> mFrames;

		float mFramesPerSecond = 15.0f;

		bool mIsLooping = false;

		bool mIsPlaying = false;

		float mCurrentTime = 0.0f;
	};

	REFLECT_EXTERN(FlipBook)

} // namespace BHive