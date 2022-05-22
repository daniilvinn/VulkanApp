#include "VulkanPipeline.h"
#include "VulkanContext.h"
#include "VulkanShader.h"

namespace vkapp {

	VulkanPipeline::VulkanPipeline()
	{
		auto swapchain = VulkanContext::Get()->GetSwapchain();
		auto device = VulkanContext::Get()->GetDevice();

		VulkanShader shaders("assets/shaders/shader_bin/shader.vert.spv", "assets/shaders/shader_bin/shader.frag.spv");
		std::vector<VkPipelineShaderStageCreateInfo> shaderinfos = shaders.GetStageCreateInfos();

		VkPipelineVertexInputStateCreateInfo vertexinputinfo = {};
		vertexinputinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexinputinfo.vertexBindingDescriptionCount = 0;
		vertexinputinfo.pVertexBindingDescriptions = nullptr;
		vertexinputinfo.vertexAttributeDescriptionCount = 0;
		vertexinputinfo.pVertexAttributeDescriptions = nullptr;

		VkPipelineInputAssemblyStateCreateInfo assemblystateinfo = {};
		assemblystateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		assemblystateinfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		assemblystateinfo.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = swapchain->GetExtent().width;
		viewport.height = swapchain->GetExtent().height;
		viewport.minDepth = 0.0f;
		viewport.minDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchain->GetExtent();

		VkPipelineViewportStateCreateInfo viewportstateinfo = {};
		viewportstateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportstateinfo.viewportCount = 1;
		viewportstateinfo.pViewports = &viewport;
		viewportstateinfo.scissorCount = 1;
		viewportstateinfo.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizerinfo = {};
		rasterizerinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerinfo.depthClampEnable = VK_FALSE;
		rasterizerinfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizerinfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerinfo.lineWidth = 1.0f;
		rasterizerinfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizerinfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizerinfo.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisamplestateinfo = {};
		multisamplestateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisamplestateinfo.sampleShadingEnable = VK_FALSE;
		multisamplestateinfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		// TODO: depth testing
		//VkPipelineDepthStencilStateCreateInfo depthstencilinfo = {};
		//depthstencilinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		//depthstencilinfo.depthTestEnable = VK_TRUE;
		//depthstencilinfo.stencilTestEnable = VK_FALSE;
		//depthstencilinfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		//depthstencilinfo.depthWriteEnable = VK_FALSE;
		
		VkPipelineColorBlendAttachmentState colorblendattachmentinfo = {};
		colorblendattachmentinfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorblendattachmentinfo.blendEnable = VK_TRUE;
		colorblendattachmentinfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorblendattachmentinfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorblendattachmentinfo.colorBlendOp = VK_BLEND_OP_ADD;
		colorblendattachmentinfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorblendattachmentinfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorblendattachmentinfo.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorblendstateinfo = {};
		colorblendstateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorblendstateinfo.attachmentCount = 1;
		colorblendstateinfo.pAttachments = &colorblendattachmentinfo;
		colorblendstateinfo.logicOpEnable = VK_FALSE;

		std::vector<VkDynamicState> dynamicstates = {
			VK_DYNAMIC_STATE_VIEWPORT
		};
		VkPipelineDynamicStateCreateInfo dynamicstateinfo = {};
		dynamicstateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicstateinfo.dynamicStateCount = dynamicstates.size();
		dynamicstateinfo.pDynamicStates = dynamicstates.data();

		VkPipelineLayoutCreateInfo layoutinfo = {};
		layoutinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutinfo.pushConstantRangeCount = 0;
		layoutinfo.pPushConstantRanges = nullptr;
		layoutinfo.setLayoutCount = 0;
		layoutinfo.pSetLayouts = nullptr;

		VK_CHECK_RESULT(vkCreatePipelineLayout(*VulkanContext::Get()->GetDevice(), &layoutinfo, nullptr, &m_Layout));
		
		m_RenderPass = swapchain->GetRenderPass();

		VkGraphicsPipelineCreateInfo pipelineinfo = {};
		pipelineinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineinfo.layout = m_Layout;
		pipelineinfo.stageCount = shaderinfos.size();
		pipelineinfo.pStages = shaderinfos.data();
		pipelineinfo.pVertexInputState = &vertexinputinfo;
		pipelineinfo.pInputAssemblyState = &assemblystateinfo;
		pipelineinfo.pViewportState = &viewportstateinfo;
		pipelineinfo.pRasterizationState = &rasterizerinfo;
		pipelineinfo.pColorBlendState = &colorblendstateinfo;
		pipelineinfo.pMultisampleState = &multisamplestateinfo;
		pipelineinfo.pDynamicState = &dynamicstateinfo;
		pipelineinfo.renderPass = m_RenderPass;
		pipelineinfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineinfo.basePipelineIndex = -1;
		
		VK_CHECK_RESULT(vkCreateGraphicsPipelines(*device, VK_NULL_HANDLE, 1, &pipelineinfo, nullptr, &m_Pipeline));
		VKAPP_LOG_INFO("Pipeline was created!");

	}

	VulkanPipeline::~VulkanPipeline()
	{
		auto vkdevice = *VulkanContext::Get()->GetDevice();
		vkDeviceWaitIdle(vkdevice);
		vkDestroyPipeline(vkdevice, m_Pipeline, nullptr);
		vkDestroyPipelineLayout(vkdevice, m_Layout, nullptr);
		vkDestroyRenderPass(vkdevice, m_RenderPass, nullptr);
	}

	void VulkanPipeline::SetRenderPass()
	{
		auto swapchain = VulkanContext::Get()->GetSwapchain();
		auto device = VulkanContext::Get()->GetDevice();

		VkAttachmentDescription colorattachment = {};
		colorattachment.format = swapchain->GetSwapchainImageFormat();
		colorattachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorattachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorattachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorattachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorattachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorattachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorattachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference attachmentref = {};
		attachmentref.attachment = 0;
		attachmentref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassdescription = {};
		subpassdescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassdescription.colorAttachmentCount = 1;
		subpassdescription.pColorAttachments = &attachmentref;

		VkRenderPassCreateInfo renderpassinfo = {};
		renderpassinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderpassinfo.attachmentCount = 1;
		renderpassinfo.pAttachments = &colorattachment;
		renderpassinfo.subpassCount = 1;
		renderpassinfo.pSubpasses = &subpassdescription;
		
		VK_CHECK_RESULT(vkCreateRenderPass(*device, &renderpassinfo, nullptr, &m_RenderPass));
	}

}