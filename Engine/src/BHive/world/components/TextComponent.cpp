#include "TextComponent.h"
#include "World/GameObject.h"

namespace BHive
{
	template <typename A>
	void Serialize(A &ar, FTextStyle &style)
	{
		ar(style.TextColor, style.Thickness, style.Smoothness, style.OutlineColor, style.OutlineThickness, style.OutlineSmoothness);
	}

	template <typename A>
	void Serialize(A &ar, FTextParams &params)
	{
		ar(params.Kerning, params.LineSpacing, params.Style);
	}

	void TextComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(Size, Params, Text);
	}

	void TextComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(Size, Params, Text);
	}

	REFLECT(TextComponent)
	{
		{
			BEGIN_REFLECT(FTextStyle)
			REFLECT_PROPERTY(TextColor)
			REFLECT_PROPERTY(Thickness)
			REFLECT_PROPERTY(Smoothness)
			REFLECT_PROPERTY(OutlineColor)
			REFLECT_PROPERTY(OutlineThickness)
			REFLECT_PROPERTY(OutlineSmoothness);
		}
		{
			BEGIN_REFLECT(FTextParams)
			REFLECT_PROPERTY(Kerning)
			REFLECT_PROPERTY(LineSpacing)
			REFLECT_PROPERTY(Style);
		}
		{
			BEGIN_REFLECT(TextComponent)
			(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY(Size) REFLECT_PROPERTY(Params) REFLECT_PROPERTY(Text) COMPONENT_IMPL();
		}
	}

} // namespace BHive