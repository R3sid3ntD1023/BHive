#include "TextureSpecification.h"
#include "utils/texture/TextureUtils.h"

namespace BHive
{
	FTextureCreateInfo::operator FTextureAPIInfo() const
	{
		FTextureAPIInfo info{};
		info.Levels = Levels;
		info.Format = TextureUtils::GetAPIFormat(InternalFormat);
		info.InternalFormat = TextureUtils::GetAPIInternalFormat(InternalFormat);
		info.Type = TextureUtils::GetAPIType(InternalFormat);
		info.FilterModes[0] = TextureUtils::GetAPIFilterMode(MinFilter);
		info.FilterModes[1] = TextureUtils::GetAPIFilterMode(MagFilter);
		info.WrapMode = TextureUtils::GetAPIWrapMode(WrapMode);
		info.CompareMode = CompareMode.has_value() ? TextureUtils::GetAPITextureCompareMode(CompareMode.value()) : 0;
		info.CompareFunc = CompareMode.has_value() ? TextureUtils::GetAPITextureCompareFunc(CompareFunc.value()) : 0;
		info.BorderColor[0] = BorderColor.r;
		info.BorderColor[1] = BorderColor.g;
		info.BorderColor[2] = BorderColor.b;
		info.BorderColor[3] = BorderColor.a;
		info.IsDepth = TextureUtils::IsDepthFormat(InternalFormat);
		info.GenerateMipMaps = GenerateMipMaps;

		return info;
	}

	REFLECT(FTextureCreateInfo)
	{
		{
			BEGIN_REFLECT_ENUM(EMinFilter)(
				ENUM_VALUE(LINEAR), ENUM_VALUE(NEAREST), ENUM_VALUE(MIPMAP_LINEAR), ENUM_VALUE(MIPMAP_NEAREST), ENUM_VALUE(MIPMAP_LINEAR_NEAREST), ENUM_VALUE(MIPMAP_NEAREST_LINEAR));
		}

		{
			BEGIN_REFLECT_ENUM(EMagFilter)
			(ENUM_VALUE(LINEAR), ENUM_VALUE(NEAREST));
		}

		{
			BEGIN_REFLECT_ENUM(EWrapMode)(ENUM_VALUE(REPEAT), ENUM_VALUE(CLAMP_TO_EDGE), ENUM_VALUE(MIRRORED_REPEAT), ENUM_VALUE(CLAMP_TO_BORDER));
		}

		{
			BEGIN_REFLECT(FTextureCreateInfo)
			REFLECT_PROPERTY(MinFilter)
			REFLECT_PROPERTY(MagFilter)
			REFLECT_PROPERTY(WrapMode);
		}
	}
} // namespace BHive
