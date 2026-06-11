#include "ShaderReflection.h"
#include <glad/glad.h>
#include <spirv_cross/spirv_cpp.hpp>
#include <spirv_cross/spirv_parser.hpp>
#include <spirv_cross/spirv_reflect.hpp>

namespace BHive
{
	void FShaderReflection::Reflect(EShaderStage stage, const std::vector<uint32_t> &source)
	{
		spirv_cross::Compiler compiler(source);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		//Get uniform buffers
		for (const auto &ubo : resources.uniform_buffers)
		{
			auto &buffer_type = compiler.get_type(ubo.base_type_id);
			auto set = compiler.get_decoration(ubo.id, spv::DecorationDescriptorSet);
			auto binding = compiler.get_decoration(ubo.id, spv::DecorationBinding);
			auto size = compiler.get_declared_struct_size(buffer_type);

			auto& ub = Sets[set].UniformBuffers[ubo.name];
			ub.Binding = binding;
			ub.Size = size;
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

		//get samplers
		for (const auto &sampler : resources.sampled_images)
		{
			auto &type = compiler.get_type(sampler.base_type_id);
			auto set = compiler.get_decoration(sampler.id, spv::DecorationDescriptorSet);
			auto binding = compiler.get_decoration(sampler.id, spv::DecorationBinding);

			auto &s = Sets[set].Samplers[sampler.name];
			s.Binding = binding;
			s.ArraySize = 1;
			s.Type = EResourceType::CombinedImageSampler;
			s.Stages |= stage;
		}

		for (const auto& storage_image : resources.storage_images)
		{
			auto &type = compiler.get_type(storage_image.base_type_id);
			auto set = compiler.get_decoration(storage_image.id, spv::DecorationDescriptorSet);
			auto binding = compiler.get_decoration(storage_image.id, spv::DecorationBinding);

			auto &s = Sets[set].Samplers[storage_image.name];
			s.Binding = binding;
			s.ArraySize = 1;
			s.Type = EResourceType::StorageImage;
			s.Stages |= stage;
		}

		for (const auto &sep_image : resources.separate_images)
		{
			auto &type = compiler.get_type(sep_image.base_type_id);
			auto set = compiler.get_decoration(sep_image.id, spv::DecorationDescriptorSet);
			auto binding = compiler.get_decoration(sep_image.id, spv::DecorationBinding);

			auto &s = Sets[set].Samplers[sep_image.name];
			s.Binding = binding;
			s.ArraySize = 1;
			s.Type = EResourceType::SeperatedImage;
			s.Stages |= stage;
		}

		for (const auto &sep_smp : resources.separate_samplers)
		{
			auto &type = compiler.get_type(sep_smp.base_type_id);
			auto set = compiler.get_decoration(sep_smp.id, spv::DecorationDescriptorSet);
			auto binding = compiler.get_decoration(sep_smp.id, spv::DecorationBinding);

			auto &s = Sets[set].Samplers[sep_smp.name];
			s.Binding = binding;
			s.ArraySize = 1;
			s.Type = EResourceType::SeperatedSampler;
			s.Stages |= stage;
		}

		//get storage buffers
		for (const auto &storage : resources.storage_buffers)
		{
			auto &type = compiler.get_type(storage.base_type_id);
			auto set = compiler.get_decoration(storage.id, spv::DecorationDescriptorSet);
			auto binding = compiler.get_decoration(storage.id, spv::DecorationBinding);
			auto size = compiler.get_declared_struct_size(type);
			
			auto &ssbo = Sets[set].StorageBuffers[storage.name];
			ssbo.Binding = binding;
			ssbo.Size = size;
			ssbo.Stages |= stage;
		}

		for (const auto &pcb : resources.push_constant_buffers)
		{
			auto &buffer_type = compiler.get_type(pcb.base_type_id);

			auto &pco = PushConstants.emplace_back();
			pco.Size = compiler.get_declared_struct_size(buffer_type);
			pco.Offset = 0;
			pco.Stages |= stage;

			for (size_t i = 0; i < buffer_type.member_types.size(); i++)
			{
				auto &member_type = compiler.get_type(buffer_type.member_types[i]);

				auto &u = pco.Members[compiler.get_member_name(buffer_type.self, i)];
				u.Type = member_type.basetype;
				u.Size = member_type.width / 8;
				u.Offset = compiler.type_struct_member_offset(buffer_type, i);
				u.Location = compiler.get_decoration(buffer_type.self, spv::DecorationLocation);
				u.Stages |= stage;
			}
		}

		for (const auto &uniform : resources.gl_plain_uniforms)
		{
			auto &type = compiler.get_type(uniform.base_type_id);

			auto &u = Uniforms[uniform.name];
			u.Type = type.basetype;
			u.Size = type.width / 8;
			u.Offset = compiler.get_decoration(uniform.id, spv::DecorationOffset);
			u.Location = compiler.get_decoration(uniform.id, spv::DecorationLocation);
			u.Stages |= stage;
		}
	}

	void FShaderReflection::AttachSemantics(const std::unordered_map<std::string, std::string> &varToSemantic)
	{
		for (auto &[set, resource] : Sets)
		{
			for (auto &[name, sampler] : resource.Samplers)
			{
				if (auto it = varToSemantic.find(name); it != varToSemantic.end())
					sampler.Semantic = it->second;
			}
			for (auto &[name, ub] : resource.UniformBuffers)
			{
				if (auto it = varToSemantic.find(name); it != varToSemantic.end())
					ub.Semantic = it->second;
			}

			for (auto &[name, ssbo] : resource.StorageBuffers)
			{
				if (auto it = varToSemantic.find(name); it != varToSemantic.end())
					ssbo.Semantic = it->second;
			}
		}
	}

	std::string FShaderReflection::to_string() const
	{
		std::string result;

		for (auto& [set, resource] : Sets)
		{
			result += fmt::format("Set {}\n", set);

			for (const auto &[name, sampler] : resource.Samplers)
			{
				result += fmt::format("\t\tSampler: {} -> Type: {}, Binding: {}, ArraySize: {};\n", name, ToString(sampler.Type), sampler.Binding, sampler.ArraySize);
			}
			for (const auto &[name, buffer] : resource.UniformBuffers)
			{
				result += fmt::format("\t\tUniform Buffer: {} ->  Binding: {}, Size: {}\n", name, buffer.Binding, buffer.Size);
				for (const auto &[uniform_name, uniform] : buffer.Members)
				{
					result += fmt::format("\t\tMember: {} - Type: {} - Size: {} - Offset: {} - Location: {}\n", uniform_name, uniform.Type, uniform.Size, uniform.Offset, uniform.Location);
				}
			}
			
			for (const auto &[name, buffer] : resource.StorageBuffers)
			{
				result += fmt::format("\t\tStorage Buffer: {} -> Binding: {}, Size: {}\n", name, buffer.Binding, buffer.Size);
			}
		}

		for (const auto &pco : PushConstants)
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

		return result;
	}

	FShaderReflection FShaderReflection::Merge(const std::unordered_map<EShaderStage, FShaderReflection> &reflection)
	{
		FShaderReflection merged{};

		for (auto &[stage, refl] : reflection)
		{
			for (auto [set, resource] : refl.Sets)
			{
				// merge samplers
				for (auto &[name, s] : resource.Samplers)
				{
					auto &dst = merged.Sets[set].Samplers[name];
					dst.Binding = s.Binding;
					dst.ArraySize = s.ArraySize;
					dst.Type = s.Type;
					dst.Stages |= s.Stages;
					dst.Semantic = s.Semantic;
				}

				// Merge UBOs
				for (auto &[name, ubo] : resource.UniformBuffers)
				{
					auto &dst = merged.Sets[set].UniformBuffers[name];
					dst.Binding = ubo.Binding;
					dst.Size = ubo.Size;
					dst.Stages |= ubo.Stages;
					dst.Semantic = ubo.Semantic;

					// ubo.Stages;
					for (auto &[memberName, member] : ubo.Members)
					{
						auto &dstMember = dst.Members[memberName];
						dstMember = member;
						dstMember.Stages |= member.Stages;
					}
				}

				// Merge SSBOs

				for (auto &[name, ssbo] : resource.StorageBuffers)
				{
					auto &dst = merged.Sets[set].StorageBuffers[name];
					dst.Binding = ssbo.Binding;
					dst.Size = ssbo.Size;
					dst.Stages |= ssbo.Stages;
					dst.Semantic = ssbo.Semantic;
				}
			}
			

			//Merge Push Constants
			for (auto & pc : refl.PushConstants)
			{
				auto &dst = merged.PushConstants.emplace_back();
				dst.Size = pc.Size;
				dst.Offset = pc.Offset;
				dst.Stages |= pc.Stages;
				dst.Members = pc.Members;
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

FShaderReflectionLookUp::FShaderReflectionLookUp(const FShaderReflection &merged)
{
	Build(merged);
}

const FReflectedResource *FShaderReflectionLookUp::FindByName(const std::string &name) const
{
	return mByName.contains(name) ? &mByName.at(name) : nullptr;
}

const FReflectedResource *FShaderReflectionLookUp::FindBySetBinding(uint32_t set, uint32_t binding) const
{
	if (mBySetBinding.contains(set))
	{
		auto s = mBySetBinding.at(set);
		if (s.contains(binding))
		{
			return &s.at(binding);
		}
	}

	return nullptr;
}

const std::vector<FReflectedResource> &FShaderReflectionLookUp::GetSetBindings(uint32_t set) const
{
	static const std::vector<FReflectedResource> empty;
	return mSets.contains(set) ? mSets.at(set) : empty;
}

void FShaderReflectionLookUp::Build(const FShaderReflection &merged)
{
	auto addResource = [&](const std::string &name, uint32_t set, const FReflectedResource &r)
	{
		mByName[name] = r;
		mBySetBinding[set][r.binding] = r;
		mSets[set].emplace_back(r);
		mMaxSet = std::max(mMaxSet, set);
	};

	for (auto &[set, resource] : merged.Sets)
	{
		for (auto &[name, ubo] : resource.UniformBuffers)
		{
			FReflectedResource r;
			r.binding = ubo.Binding;
			r.kind = ubo.Type;
			r.size = ubo.Size;
			r.name = name;
			r.Semantic = ubo.Semantic;
			addResource(name, set, r);
		}

		for (auto &[name, ssbo] : resource.StorageBuffers)
		{
			FReflectedResource r;
			r.binding = ssbo.Binding;
			r.kind = ssbo.Type;
			r.size = ssbo.Size;
			r.name = name;
			r.Semantic = ssbo.Semantic;
			addResource(name, set, r);
		}

		for (auto &[name, smp] : resource.Samplers)
		{
			FReflectedResource r;
			r.binding = smp.Binding;
			r.kind = smp.Type;
			r.name = name;
			r.Semantic = smp.Semantic;
			addResource(name, set, r);
		}
	}

	// sort each set by binding index
	for (auto &[set, vec] : mSets)
	{
		std::sort(vec.begin(), vec.end(), [](auto &a, auto &b) { return a.binding < b.binding; });
	}
}
} // namespace BHive