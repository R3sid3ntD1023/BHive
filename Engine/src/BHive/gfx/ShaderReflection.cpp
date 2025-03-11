#include "ShaderReflection.h"
#include <glad/glad.h>

namespace BHive
{
	FShaderReflectionData::FShaderReflectionData(uint32_t program)
	{
		GetUniforms(program);
		GetSamplers(program);
		GetUniformBuffers(program);
		GetStorageBuffers(program);
	}

	void FShaderReflectionData::GetUniforms(uint32_t program)
	{
		GLenum properties[] = {GL_NAME_LENGTH, GL_TYPE, GL_ARRAY_SIZE, GL_OFFSET, GL_LOCATION};
		GLint data[5] = {};
		GLint count = 0;
		GLchar name[512];

		glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &count);

		for (int i = 0; i < count; i++)
		{
			glGetProgramResourceiv(program, GL_UNIFORM, i, 5, properties, 5, nullptr, data);

			glGetProgramResourceName(program, GL_UNIFORM, i, data[0], nullptr, name);

			FUniform uniform{data[1], data[2], data[3], data[4]};
			Uniforms.emplace(name, uniform);
		}
	}

	void FShaderReflectionData::GetUniformBuffers(uint32_t program)
	{
		GLenum properties[] = {GL_NAME_LENGTH, GL_NUM_ACTIVE_VARIABLES, GL_BUFFER_BINDING, GL_BUFFER_DATA_SIZE};
		GLint data[4] = {};
		GLchar name[512];
		GLchar member_name[512];

		GLenum member = GL_ACTIVE_VARIABLES;
		GLenum member_props[] = {GL_NAME_LENGTH, GL_TYPE, GL_ARRAY_SIZE, GL_OFFSET, GL_LOCATION};
		GLint member_data[5] = {};

		GLint count = 0;
		glGetProgramInterfaceiv(program, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &count);

		for (int i = 0; i < count; i++)
		{

			glGetProgramResourceiv(program, GL_UNIFORM_BLOCK, i, 4, properties, 4, nullptr, data);

			glGetProgramResourceName(program, GL_UNIFORM_BLOCK, i, data[0], nullptr, name);

			std::vector<GLint> indices(data[1]);
			glGetProgramResourceiv(program, GL_UNIFORM_BLOCK, i, 1, &member, indices.size(), nullptr, indices.data());

			FUniformBufferData uniform_buffer{data[2], data[3]};

			for (auto &idx : indices)
			{
				glGetProgramResourceiv(program, GL_UNIFORM, idx, 5, member_props, 5, nullptr, member_data);

				glGetProgramResourceName(program, GL_UNIFORM, idx, member_data[0], nullptr, member_name);

				FUniform uniform{member_data[1], member_data[2], member_data[3], member_data[4]};
				uniform_buffer.Uniforms.emplace(member_name, uniform);
			}

			UniformBuffers.emplace(name, uniform_buffer);
		}
	}
	void FShaderReflectionData::GetStorageBuffers(uint32_t program)
	{
		GLint count = 0;
		GLenum properties[] = {GL_NAME_LENGTH, GL_BUFFER_BINDING, GL_BUFFER_DATA_SIZE};
		GLint data[3] = {};
		GLchar name[512];

		glGetProgramInterfaceiv(program, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &count);

		for (int i = 0; i < count; i++)
		{
			glGetProgramResourceiv(program, GL_SHADER_STORAGE_BLOCK, i, 3, properties, 3, nullptr, data);

			glGetProgramResourceName(program, GL_SHADER_STORAGE_BLOCK, i, data[0], nullptr, name);

			StorageBuffers.emplace(name, FStorageBuffer{data[1], data[2]});
		}
	}

	void FShaderReflectionData::GetSamplers(uint32_t program)
	{
	}

} // namespace BHive