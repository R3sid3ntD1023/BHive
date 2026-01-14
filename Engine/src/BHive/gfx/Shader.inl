#pragma once

#include "Shader.h"
#include "RenderCommand.h"
#include "RendererAPI.h"

namespace BHive
{
	template <typename T>
	inline void Shader::SetUniform(const std::string &name, const T &val)
	{
		auto offset = mReflectionData.Uniforms[name].Offset;
		auto api = RenderCommand::GetAPI();
		api->SubmitCommand([=](const FVulkanFrameData &data) { data.CommandBuffer.pushConstants(mPipelineLayout, vk::ShaderStageFlagBits::eAll, offset, val); });
	}

	template <>
	inline void Shader::SetUniform(const std::string &name, const int32_t &val)
	{
		auto offset = mReflectionData.Uniforms[name].Offset;
		auto api = RenderCommand::GetAPI();
		api->SubmitCommand([=](const FVulkanFrameData &data) { data.CommandBuffer.pushConstants(*mPipelineLayout, vk::ShaderStageFlagBits::eAll, offset, val); });
	}

	template <typename T, glm::length_t L, glm::qualifier Q>
	inline void Shader::SetUniform(const std::string &name, const glm::vec<L, T, Q> &val)
	{
		auto offset = mReflectionData.Uniforms[name].Offset;
		auto api = RenderCommand::GetAPI();
		api->SubmitCommand([=](const FVulkanFrameData &data) { data.CommandBuffer.pushConstants(mPipelineLayout, vk::ShaderStageFlagBits::eAll, offset, {val.x}); });
	}	

	template <typename T, glm::length_t C, glm::length_t R, glm::qualifier Q>
	inline void Shader::SetUniform(const std::string &name, const glm::mat<C, R, T, Q> &val)
	{
		auto offset = mReflectionData.Uniforms[name].Offset;
		auto api = RenderCommand::GetAPI();
		api->SubmitCommand([=](const FVulkanFrameData &data) { data.CommandBuffer.pushConstants(mPipelineLayout, vk::ShaderStageFlagBits::eAll, offset, {val.x}); });
	}
}