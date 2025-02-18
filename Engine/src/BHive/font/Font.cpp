#include "Font.h"
#include <freetype/freetype.h>
#include <glad/glad.h>
#include <gfx/Texture.h>

namespace BHive
{
	Font::Font(const char *filename, int fontSize)
		: mFontSize(fontSize)
	{
		Initialize(filename);
	}

	Font::~Font()
	{
	}

	const Glyph &Font::GetCharacter(uint8_t c) const
	{
		return mCharacters.at(c);
	}

	float Font::GetDeviceScale() const
	{
		return mDeviceScale;
	}

	void Font::Initialize(const char *filename)
	{
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			ASSERT(false, "Failed to init freetype libary");
			return;
		}

		FT_Face face;
		auto error = FT_New_Face(ft, filename, 0, &face);
		if (error)
		{
			ASSERT(false);
			return;
		}

		float scale = (float)face->units_per_EM / (float)(face->height + 4 * mFontSize);
		FT_Set_Pixel_Sizes(face, 0, mFontSize * scale);

		int line_height = 0;

		int max_dim = (1 + (face->size->metrics.height >> 6)) * ceilf(sqrtf(128));
		int tex_width = 1;
		while (tex_width < max_dim)
			tex_width <<= 1;
		int tex_height = tex_width;

		char *pixels = (char *)calloc(tex_width * tex_height, 1);
		int x = 0, y = 0;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte alignment restriction

		FT_GlyphSlot slot = face->glyph;
		for (uint8_t c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				continue;

			if (FT_Render_Glyph(slot, FT_RENDER_MODE_SDF))
				continue;

			auto buffer = slot->bitmap.buffer;

			if (x + slot->bitmap.width >= tex_width)
			{
				x = 0;
				y += (face->size->metrics.height >> 6) + 1;
			}

			auto width = slot->bitmap.width;
			auto rows = slot->bitmap.rows;
			for (uint32_t row = 0; row < rows; row++)
			{
				for (uint32_t col = 0; col < width; col++)
				{
					int _x = x + col;
					int _y = y + row;
					pixels[_y * tex_width + _x] = buffer[row * slot->bitmap.pitch + col];
				}
			}

			Glyph character;
			character.Position = {x, y};
			character.Advance = slot->advance.x;
			character.Size = {width, rows};
			character.Bearing = {slot->bitmap_left, slot->bitmap_top};
			character.Bounds = {character.Position, character.Position + character.Size};

			mCharacters.emplace(c, character);

			line_height = glm::max((int)(face->size->metrics.height) >> 6, line_height);
			x += slot->bitmap.width + 1;
		}

		mAscender = face->ascender >> 6;
		mDescender = face->descender >> 6;
		mLineHeight = line_height;

		double scale_x = face->size->metrics.x_scale / 65536.0;
		double scale_y = face->size->metrics.y_scale / 65536.0;
		mDeviceScale = 1.f / (mAscender - mDescender);

		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		char *data = (char *)calloc(tex_width * tex_height * 4, 1);
		for (int i = 0; i < (tex_width * tex_height); i++)
		{
			data[i * 4 + 0] |= pixels[i];
			data[i * 4 + 1] |= pixels[i];
			data[i * 4 + 2] |= pixels[i];
			data[i * 4 + 3] = 0xff;
		}

		FTextureSpecification specification;
		specification.mMagFilter = EMagFilter::LINEAR;
		specification.mMinFilter = EMinFilter::LINEAR;
		specification.mWrapMode = EWrapMode::CLAMP_TO_EDGE;
		specification.mFormat = EFormat::RGBA8;
		specification.mChannels = 4;
		mTextureAtlas = Texture2D::Create(data, tex_width, tex_height, specification);

		free(pixels);
		free(data);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

} // namespace BHive
