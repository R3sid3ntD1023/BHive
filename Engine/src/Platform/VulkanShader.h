#pragma once

#include "core/Core.h"
#include "gfx/ShaderReflection.h"
#include "VulkanCore.h"

namespace BHive
{
	enum EShaderType : uint8_t
	{
		ShaderType_None = 0,
		ShaderType_Vertex,
		ShaderType_Fragment,
		ShaderType_Compute,
		ShaderType_Geometry,
	};

	using ShaderReflectionData = std::unordered_map<EShaderType, FShaderReflectionData>;

	class VulkanShader
	{
	public:
		VulkanShader(const std::filesystem::path &path);
		VulkanShader(const std::string &name, const std::string &vertex_shader, const std::string &fragment_shader);
		VulkanShader(const std::string &name, const std::string &comp_shader);

		virtual ~VulkanShader();

		virtual void Compile(bool recompile = false);
		virtual void Recompile();

		virtual const std::string &GetName() const { return mName; }
		virtual void Dispatch(uint32_t w, uint32_t h, uint32_t d = 1);
		virtual const ShaderReflectionData &GetRelectionData() const { return mReflectionData; }

	private:
		void PreProcess(const std::string &source);
		void Reflect();

		void GetOrCreateVulkanBinaries(bool recompile = false);
		void CreateCacheDirectoryIfNeeded();

	private:
		std::string mName;
		std::filesystem::path mFilePath;
		ShaderReflectionData mReflectionData;

		std::unordered_map<uint32_t, std::string> mSources;
		std::unordered_map<uint32_t, std::vector<uint32_t>> mVulkanSpirv;
		std::vector<VkPipelineShaderStageCreateInfo> mShaderStages;
	};

} // namespace BHive
