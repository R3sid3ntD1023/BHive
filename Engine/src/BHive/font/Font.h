#pragma once

#include "core/Core.h"
#include <glm/glm.hpp>

namespace BHive
{
	class Texture2D;

	struct GlyphBounds
	{
		glm::vec2 Min;
		glm::vec2 Max;
	};

	struct Glyph
	{
		uint32_t Advance;
		glm::vec2 Position;
		glm::vec2 Size, Bearing;
		GlyphBounds Bounds;
	};

	class Font
	{
	private:
		/* data */
	public:
		Font(const char *filename, int fontSize);
		~Font();

		const Glyph &GetCharacter(uint8_t c) const;

		float GetDeviceScale() const;

		int GetLineHeight() const { return mLineHeight; }

		const Ref<Texture2D> &GetAtlas() const { return mTextureAtlas; }

	private:
		void Initialize(const char *filename);

	private:
		int mFontSize = 0;
		float mAscender = 0;
		float mDescender = 0;
		int mLineHeight = 0;
		float mDeviceScale;

		std::unordered_map<uint8_t, Glyph> mCharacters;
		Ref<Texture2D> mTextureAtlas;
	};

} // namespace BHive
