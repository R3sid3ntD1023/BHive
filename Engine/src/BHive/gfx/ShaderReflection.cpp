#include "ShaderReflection.h"
#include <glad/glad.h>
#include <spirv_cross/spirv_cpp.hpp>

namespace BHive
{
	void FShaderReflectionData::Reflect(const std::vector<uint32_t> &source)
	{
		spirv_cross::Compiler compiler(source);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();
		for (const auto &uniform : resources.uniform_buffers)
		{
			auto &buffer = compiler.get_type(uniform.base_type_id);

			FUniformBufferData buffer_data;
			buffer_data.Binding = compiler.get_decoration(uniform.id, spv::DecorationBinding);
			buffer_data.Size = compiler.get_declared_struct_size(buffer);
			auto member_count = buffer.member_types.size();
			for (size_t i = 0; i < member_count; i++)
			{
				auto &member = buffer.member_types[i];
				auto &type = compiler.get_type(member);

				FUniform uniform_data;
				uniform_data.Type = type.basetype;
				uniform_data.Size = type.width / 8;
				uniform_data.Offset = compiler.type_struct_member_offset(buffer, i);
				uniform_data.Location = compiler.get_decoration(member, spv::DecorationLocation);
				buffer_data.Uniforms[compiler.get_member_name(buffer.self, i)] = uniform_data;
			}
			UniformBuffers[uniform.name] = buffer_data;
		}

		for (auto &uniform : resources.gl_plain_uniforms)
		{
			auto &type = compiler.get_type(uniform.base_type_id);
			FUniform uniform_data;
			uniform_data.Type = type.basetype;
			uniform_data.Size = type.width / 8;
			uniform_data.Offset = compiler.get_decoration(uniform.id, spv::DecorationOffset);
			uniform_data.Location = compiler.get_decoration(uniform.id, spv::DecorationLocation);
			Uniforms[uniform.name] = uniform_data;
		}

		for (auto &sampler : resources.sampled_images)
		{
			auto &type = compiler.get_type(sampler.base_type_id);
			FSampler2D sampler_data;
			sampler_data.Binding = compiler.get_decoration(sampler.id, spv::DecorationBinding);
			Samplers[sampler.name] = sampler_data;
		}

		for (auto &storage : resources.storage_buffers)
		{
			auto &type = compiler.get_type(storage.base_type_id);
			FStorageBuffer storage_data;
			storage_data.Binding = compiler.get_decoration(storage.id, spv::DecorationBinding);
			storage_data.Size = compiler.get_declared_struct_size(type);
			StorageBuffers[storage.name] = storage_data;
		}
	}

	std::string FShaderReflectionData::to_string() const
	{
		std::string result;
		for (const auto &[name, sampler] : Samplers)
		{
			result += fmt::format("\t\tSampler: {} - Binding: {}\n", name, sampler.Binding);
		}
		for (const auto &[name, buffer] : UniformBuffers)
		{
			result += fmt::format("\t\tUniform Buffer: {} - Binding: {} - Size: {}\n", name, buffer.Binding, buffer.Size);
			for (const auto &[uniform_name, uniform] : buffer.Uniforms)
			{
				result += fmt::format(
					"\t\tUniform: {} - Type: {} - Size: {} - Offset: {} - Location: {}\n", uniform_name, uniform.Type,
					uniform.Size, uniform.Offset, uniform.Location);
			}
		}
		for (const auto &[name, uniform] : Uniforms)
		{
			result += fmt::format(
				"\t\tUniform: {} - Type: {} - Size: {} - Offset: {} - Location: {}\n", name, uniform.Type, uniform.Size,
				uniform.Offset, uniform.Location);
		}
		for (const auto &[name, buffer] : StorageBuffers)
		{
			result += fmt::format("\t\tStorage Buffer: {} - Binding: {} - Size: {}\n", name, buffer.Binding, buffer.Size);
		}
		return result;
	}
} // namespace BHive