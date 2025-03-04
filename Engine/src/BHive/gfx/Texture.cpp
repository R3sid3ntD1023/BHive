#include "Texture.h"
#include <glad/glad.h>

namespace BHive
{
	REFLECT(Texture)
	{
		BEGIN_REFLECT(Texture)
		REFLECT_PROPERTY_READ_ONLY("Width", GetWidth)
		REFLECT_PROPERTY_READ_ONLY("Height", GetHeight);
	}

} // namespace BHive