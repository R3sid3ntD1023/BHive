#include "Texture.h"

namespace BHive
{

	REFLECT(Texture)
	{
		BEGIN_REFLECT(Texture) REFLECT_PROPERTY_READ_ONLY("Width", GetWidth) REFLECT_PROPERTY_READ_ONLY("Height", GetHeight);
		rttr::type::register_wrapper_converter_for_base_classes<Ref<Texture>>();
	}
} // namespace BHive