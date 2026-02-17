#include "ShaderReflection.h"
#include <glad/glad.h>
#include <spirv_cross/spirv_cpp.hpp>

namespace BHive
{
	void FShaderReflection::Reflect(EShaderStage stage, const std::vector<uint32_t> &source)
	{
		spirv_cross::Compiler compiler(source);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		for (const auto &ubo : resources.uniform_buffers)
		{
			auto &buffer_type = compiler.get_type(ubo.base_type_id);

			auto& ub = UniformBuffers[ubo.name];
			ub.Binding = compiler.get_decoration(ubo.id, spv::DecorationBinding);
			ub.Size = compiler.get_declared_struct_size(buffer_type);
			ub.Stages |= stage;

			for (size_t i = 0; i < buffer_type.member_types.size(); i++)
			{
				auto &member = buffer_type.member_types[i];
				auto &member_type = compiler.get_type(member);

				auto &u = ub.Members[compiler.get_member_name(buffer_type.self, i)];
				u.Type = member_type.basetype;
				u.Size = member_type.width / 8;
				u.Offset = compiler.type_struct_member_offset(buffer_type, i);
				u.Location = compiler.get_decoration(member, spv::DecorationLocation);
				u.Stages |= stage;
			}
		}

		for (const auto &pcb : resources.push_constant_buffers)
		{
			auto &buffer_type = compiler.get_type(pcb.base_type_id);

			auto& pco  = PushConstants.emplace_back();
			pco.Size = compiler.get_declared_struct_size(buffer_type);
			pco.Offset = 0;
			pco.Stages |= stage;

			for (size_t i = 0; i < buffer_type.member_types.size(); i++)
			{
				auto &member_type = compiler.get_type(buffer_type.member_types[i]);

				auto& u = pco.Members[compiler.get_member_name(buffer_type.self, i)];
				u.Type = member_type.basetype;
				u.Size = member_type.width / 8;
				u.Offset = compiler.type_struct_member_offset(buffer_type, i);
				u.Location = compiler.get_decoration(buffer_type.self, spv::DecorationLocation);
				u.Stages |= stage;
			}
		}

		for (auto &uniform : resources.gl_plain_uniforms)
		{
			auto &type = compiler.get_type(uniform.base_type_id);
			
			auto& u = Uniforms[uniform.name];
			u.Type = type.basetype;
			u.Size = type.width / 8;
			u.Offset = compiler.get_decoration(uniform.id, spv::DecorationOffset);
			u.Location = compiler.get_decoration(uniform.id, spv::DecorationLocation);
			u.Stages |= stage;
		}

		for (auto &sampler : resources.sampled_images)
		{
			auto &type = compiler.get_type(sampler.base_type_id);
			auto &s = Samplers[sampler.name];
			s.Set = compiler.get_decoration(sampler.id, spv::DecorationDescriptorSet);
			s.Binding = compiler.get_decoration(sampler.id, spv::DecorationBinding);
			s.Stages |= stage;
		}

		for (auto &storage : resources.storage_buffers)
		{
			auto &type = compiler.get_type(storage.base_type_id);
			
			auto &sb = StorageBuffers[storage.name];
			sb.Set = compiler.get_decoration(storage.id, spv::DecorationDescriptorSet);
			sb.Binding = compiler.get_decoration(storage.id, spv::DecorationBinding);
			sb.Size = compiler.get_declared_struct_size(type);
			sb.Stages |= stage;
		}
	}

	std::string FShaderReflection::to_string() const
	{
		std::string result;

		for (const auto &[name, sampler] : Samplers)
		{
			result += fmt::format("\t\tSampler: {} - Set: {} - Binding: {}\n", name, sampler.Set, sampler.Binding);
		}
		for (const auto &[name, buffer] : UniformBuffers)
		{
			result += fmt::format("\t\tUniform Buffer: {} - Set: {} - Binding: {} - Size: {}\n", name, buffer.Set, buffer.Binding, buffer.Size);
			for (const auto &[uniform_name, uniform] : buffer.Members)
			{
				result += fmt::format("\t\tMember: {} - Type: {} - Size: {} - Offset: {} - Location: {}\n", uniform_name, uniform.Type, uniform.Size, uniform.Offset, uniform.Location);
			}
		}
		for (const auto & pco : PushConstants)
		{
			result += fmt::format("\t\tPush Constant: Size: {}\n", pco.Size);
			for (const auto &[uniform_name, uniform] : pco.Members)
			{
				result += fmt::format("\t\tMember: {} - Type: {} - Size: {} - Offset: {} - Location: {}\n", uniform_name, uniform.Type, uniform.Size, uniform.Offset, uniform.Location);
			}
		}
		for (const auto &[name, uniform] : Uniforms)
		{
			result += fmt::format("\t\tUniform: {} - Type: {} - Size: {} - Offset: {} - Location: {}\n", name, uniform.Type, uniform.Size, uniform.Offset, uniform.Location);
		}
		for (const auto &[name, buffer] : StorageBuffers)
		{
			result += fmt::format("\t\tStorage Buffer: {} - Set: {} - Binding: {} - Size: {}\n", name, buffer.Set, buffer.Binding, buffer.Size);
		}
		return result;
	}

	FShaderReflection FShaderReflection::Merge(const std::unordered_map<EShaderStage, FShaderReflection> &reflection)
	{
		FShaderReflection merged{};

		for (auto &[stage, refl] : reflection)
		{
			// merge samplers
			for (auto &[name, s] : refl.Samplers)
			{
				auto &dst = merged.Samplers[name];
				dst.Set = s.Set;
				dst.Binding = s.Binding;
				dst.Stages |= s.Stages;
			}

			// Merge UBOs
			for (auto &[name, ubo] : refl.UniformBuffers)
			{
				auto &dst = merged.UniformBuffers[name];
				dst.Set = ubo.Set;
				dst.Binding = ubo.Binding;
				dst.Size = ubo.Size;
				dst.Stages |= ubo.Stages;

				// ubo.Stages;
				for (auto &[memberName, member] : ubo.Members)
				{
					auto &dstMember = dst.Members[memberName];
					dstMember = member;
					dstMember.Stages |= member.Stages;
				}
			} // Merge SSBOs

			for (auto &[name, ssbo] : refl.StorageBuffers)
			{
				auto &dst = merged.StorageBuffers[name];
				dst.Set = ssbo.Set;
				dst.Binding = ssbo.Binding;
				dst.Size = ssbo.Size;
				dst.Stages |= ssbo.Stages;
			}

			// // Merge plain uniforms
			for (auto &[name, uniform] : refl.Uniforms)
			{
				auto &dst = merged.Uniforms[name];
				dst = uniform;
				dst.Stages |= uniform.Stages;
			}
		}

		return merged;
	}
} // namespace BHive