#include "FontManager.h"
#include "Font.h"

namespace BHive
{
	void FontManager::AddFont(const std::string &name, const Ref<Font> &font)
	{
		mFonts.emplace(name, font);
		if (!mDefaultFont)
		{
			mDefaultFont = font;
		}
	}

	Ref<Font> FontManager::AddFontFromFile(const std::filesystem::path &filename, int fontSize)
	{
		auto font = CreateRef<Font>(filename.string().c_str(), fontSize);
		AddFont(filename.stem().string(), font);
		return font;
	}

	void FontManager::SetDefaultFont(const Ref<Font> &font)
	{
		mDefaultFont = font;
	}
	FontManager &FontManager::Get()
	{
		static FontManager manager;
		return manager;
	}
} // namespace BHive