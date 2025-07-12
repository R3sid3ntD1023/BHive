#include "Font.h"
#include <glad/glad.h>
#include <gfx/textures/Texture2D.h>
#include "MSDFData.h"

using namespace msdf_atlas;

namespace BHive
{

	template <typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
	Ref<Texture2D> CreateAndCacheAtlas(
		float fontSize, const std::vector<msdf_atlas::GlyphGeometry> &glyphs, const msdf_atlas::FontGeometry &fontGeomerty,
		uint32_t w, uint32_t h)
	{

		GeneratorAttributes attributes;
		attributes.config.overlapSupport = true;
		attributes.scanlinePass = true;

		ImmediateAtlasGenerator<S, N, GenFunc, BitmapAtlasStorage<T, N>> generator(w, h);
		generator.setAttributes(attributes);
		generator.setThreadCount(2);
		generator.generate(glyphs.data(), (int)glyphs.size());

		msdfgen::BitmapConstRef<T, N> bitmap = (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();

		FTextureSpecification specification{};
		specification.InternalFormat = EFormat::RGB8;
		specification.Channels = 3;

		Ref<Texture2D> texture = CreateRef<Texture2D>(w, h, specification, bitmap.pixels, w * h * 3);
		return texture;
	};

	Font::Font(const char *filename, int fontSize)
		: mData(new MSDFData()),
		  mFontSize(fontSize)
	{
		Initialize(filename);
	}

	Font::~Font()
	{
		delete mData;
	}

	void Font::Initialize(const char *filename)
	{
		msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype();
		if (!ft)
			return;

		msdfgen::FontHandle *font = msdfgen::loadFont(ft, filename);
		if (!font)
			return;

		struct CharsetRange
		{
			uint32_t begin, end;
		};

		static const CharsetRange char_set_ranges[] = {0x0020, 0x00FF};

		msdf_atlas::Charset charset;
		for (auto &range : char_set_ranges)
		{
			for (uint32_t c = range.begin; c <= range.end; c++)
				charset.add(c);
		}

		mData->FontGeometry = FontGeometry(&mData->Glyphs);
		int loaded_glyphs = mData->FontGeometry.loadCharset(font, 1, charset);
		LOG_INFO("Loaded {} glyphs", loaded_glyphs);

		double emSize = 40.0;
		TightAtlasPacker atlas_packer;
		atlas_packer.setPixelRange(2.0);
		atlas_packer.setMiterLimit(1.0);
		atlas_packer.setInnerPixelPadding(0);
		atlas_packer.setOuterPixelPadding(0);
		atlas_packer.setScale(emSize);
		int remaining = atlas_packer.pack(mData->Glyphs.data(), (int)mData->Glyphs.size());
		ASSERT(remaining == 0);

		int w, h;
		atlas_packer.getDimensions(w, h);
		emSize = atlas_packer.getScale();

#define DEFAULT_ANGLE_THRESHOLD 3.0
#define LCG_MULTIPLIER 6364136223846793005ull
#define LCG_INCREMENT 1442695040888963407ull
#define THREAD_COUNT 8

		uint64_t coloringSeed = 0;
		unsigned long long glyphSeed = coloringSeed;
		for (GlyphGeometry &glyph : mData->Glyphs)
		{
			glyphSeed *= LCG_MULTIPLIER;
			glyph.edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
		}

		mTextureAtlas = CreateAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>(
			(float)emSize, mData->Glyphs, mData->FontGeometry, w, h);

		msdfgen::destroyFont(font);
		msdfgen::deinitializeFreetype(ft);
	}

} // namespace BHive
