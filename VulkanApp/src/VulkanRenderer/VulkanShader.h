#pragma once

#include <Core/Base.h>
#include <vector>

#include <vulkan/vulkan.h>

namespace vkapp {

	class VulkanShader
	{
	public:
		VulkanShader(const std::string& vertSrc, const std::string& fragSrc);
		~VulkanShader();

		std::vector<VkPipelineShaderStageCreateInfo>& GetStageCreateInfos() { return m_CreateInfos; };

	public:
		static std::vector<char> ReadGlslBinFile(const std::string& filepath);

	private:
		std::vector<VkPipelineShaderStageCreateInfo> m_CreateInfos;
	};

}