#include "PropertyUniform.h"
#include <glad/glad.h>

namespace BHive
{
	FUniformProperty *CreateProperty(const std::string &name, int32_t type)
	{
		switch (type)
		{
		case GL_INT:
			return new TUniformProperty<int32_t>(name);
		case GL_UNSIGNED_INT:
			return new TUniformProperty<uint32_t>(name);
		case GL_FLOAT:
			return new TUniformProperty<float>(name);
		case GL_FLOAT_VEC2:
			return new TUniformProperty<glm::vec2>(name);
		case GL_FLOAT_VEC3:
			return new TUniformProperty<glm::vec3>(name);
		case GL_FLOAT_VEC4:
			return new TUniformProperty<glm::vec4>(name);
		case GL_FLOAT_MAT2:
			return new TUniformProperty<glm::mat2>(name);
		case GL_FLOAT_MAT3:
			return new TUniformProperty<glm::mat3>(name);
		case GL_FLOAT_MAT4:
			return new TUniformProperty<glm::mat4>(name);
		default:
			break;
		}
		return nullptr;
	}

} // namespace BHive