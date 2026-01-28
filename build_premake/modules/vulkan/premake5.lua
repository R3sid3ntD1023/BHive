VULKAN_SDK = os.getenv("VULKAN_SDK")

if VULKAN_SDK == nil then
	error("VULKAN_SDK environment variable is not set. Please install the Vulkan SDK and set the VULKAN_SDK environment variable.")
end

IncludeDir["vulkan"] = "%{VULKAN_SDK}/Include"

--Vulkan libs
Library["vulkan"] = "%{VULKAN_SDK}/Lib/vulkan-1.lib"
Library["shaderc"] = "%{VULKAN_SDK}/Lib/spirv-cross-shaderc.lib"
Library["shaderc_debug"] = "%{VULKAN_SDK}/Lib/spirv-cross-shaderc-d.lib"
Library["spirv_cross_core"] = "%{VULKAN_SDK}/Lib/spirv-cross-core.lib"
Library["spirv_cross_debug"] = "%{VULKAN_SDK}/Lib/spirv-cross-cored.lib"
Library["spirv_cross_glsl"] = "%{VULKAN_SDK}/Lib/spirv-cross-glsl.lib"
Library["spirv_cross_glsl_debug"] = "%{VULKAN_SDK}/Lib/spirv-cross-glsld.lib"
