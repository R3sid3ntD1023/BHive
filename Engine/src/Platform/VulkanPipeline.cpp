#include "VulkanAPI.h"
#include "VulkanPipeline.h"
#include <GLFW/glfw3.h>

namespace BHive
{
	VulkanPipeline::VulkanPipeline()
	{
		std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

		VkPipelineDynamicStateCreateInfo dynamic_state{};
		dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_state.dynamicStateCount = dynamic_states.size();
		dynamic_state.pDynamicStates = dynamic_states.data();

		VkPipelineVertexInputStateCreateInfo vertex_info{};
		vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_info.vertexAttributeDescriptionCount = 0;
		vertex_info.pVertexAttributeDescriptions = nullptr;
		vertex_info.vertexBindingDescriptionCount = 0;
		vertex_info.pVertexBindingDescriptions = nullptr;

		VkPipelineInputAssemblyStateCreateInfo input_info{};
		input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		input_info.primitiveRestartEnable = VK_FALSE;
		input_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		int width = 0, height = 0;
		glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = (float)width;
		viewport.height = (float)height;
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = {(unsigned)width, (unsigned)height};

		VkPipelineViewportStateCreateInfo viewport_info{};
		viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_info.viewportCount = 1;
		viewport_info.scissorCount = 1;
		viewport_info.pScissors = &scissor;
		viewport_info.pViewports = &viewport;

		VkPipelineRasterizationStateCreateInfo rasterizer_info{};
		rasterizer_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer_info.depthBiasEnable = VK_FALSE;
		rasterizer_info.rasterizerDiscardEnable = VK_FALSE;
		rasterizer_info.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer_info.lineWidth = 1.f;
		rasterizer_info.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer_info.depthBiasEnable = VK_FALSE;
		rasterizer_info.depthBiasConstantFactor = 0.f; // optional
		rasterizer_info.depthBiasClamp = 0.0f;		   // optional
		rasterizer_info.depthBiasSlopeFactor = 0.0f;   // optional

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.f;			// optional
		multisampling.pSampleMask = nullptr;			// optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // optional
		multisampling.alphaToOneEnable = VK_FALSE;		// optional

		VkPipelineLayoutCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		create_info.setLayoutCount = 0;			   // optional
		create_info.pSetLayouts = nullptr;		   // optional
		create_info.pushConstantRangeCount = 0;	   // optional
		create_info.pPushConstantRanges = nullptr; // optional

		auto device = VulkanAPI::GetAPI().GetDevice();
		VK_ASSERT(vkCreatePipelineLayout(device, &create_info, nullptr, &mPipelineLayout), "Failed to create pipeline!");
	}

	VulkanPipeline::~VulkanPipeline()
	{
		auto device = VulkanAPI::GetAPI().GetDevice();
		vkDestroyPipelineLayout(device, mPipelineLayout, nullptr);
	}
} // namespace BHive