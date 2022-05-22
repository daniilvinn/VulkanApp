#include "VulkanShader.h"
#include <fstream>

#include "VulkanContext.h"

namespace vkapp {

	VulkanShader::VulkanShader(const std::string& vertSrc, const std::string& fragSrc)
	{
		auto device = VulkanContext::Get()->GetDevice();

		// TODO: move to Utils namespace
		// Reading glsl binaries doesn't differ from reading other binaries
		auto vertbin = ReadGlslBinFile(vertSrc);
		auto fragbin = ReadGlslBinFile(fragSrc);

		VkShaderModuleCreateInfo vertmodulecreateinfo = {};
		vertmodulecreateinfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vertmodulecreateinfo.pNext = nullptr;
		vertmodulecreateinfo.codeSize = vertbin.size();
		vertmodulecreateinfo.pCode = reinterpret_cast<const uint32_t*>(vertbin.data());
		
		VkShaderModuleCreateInfo fragmodulecreateinfo = {};
		fragmodulecreateinfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		fragmodulecreateinfo.pNext = nullptr;
		fragmodulecreateinfo.codeSize = fragbin.size();
		fragmodulecreateinfo.pCode = reinterpret_cast<const uint32_t*>(fragbin.data());

		VkShaderModule vertmodule;
		vkCreateShaderModule(*device, &vertmodulecreateinfo, nullptr, &vertmodule);

		VkShaderModule fragmodule;
		vkCreateShaderModule(*device, &fragmodulecreateinfo, nullptr, &fragmodule);

		VkPipelineShaderStageCreateInfo vertinfo = {};
		vertinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertinfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertinfo.module = vertmodule;
		vertinfo.pName = "main";

		VkPipelineShaderStageCreateInfo fraginfo = {};
		fraginfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fraginfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fraginfo.module = fragmodule;
		fraginfo.pName = "main";

		m_CreateInfos.push_back(vertinfo);
		m_CreateInfos.push_back(fraginfo);

	}

	VulkanShader::~VulkanShader()
	{

	}

	std::vector<char> VulkanShader::ReadGlslBinFile(const std::string& filepath)
	{
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);

		if (!file.is_open())
			throw std::runtime_error("Failed to open " + filepath + " file.");

		// reading
		size_t filesize = (size_t)file.tellg();
		std::vector<char> buffer(filesize);
		file.seekg(0);
		file.read(buffer.data(), filesize);

		file.close();

		return buffer;

	}

}