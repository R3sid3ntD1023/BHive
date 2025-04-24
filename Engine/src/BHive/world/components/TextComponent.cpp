#include "TextComponent.h"

#include "GameObject.h"

namespace BHive
{
	void TextComponent::Render()
	{
		QuadRenderer::DrawText(Size, Text, Params, GetWorldTransform());
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
			(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR()
				REFLECT_PROPERTY(Size) 
				REFLECT_PROPERTY(Params)
				REFLECT_PROPERTY(Text)
				COMPONENT_IMPL();
		}
	}
}