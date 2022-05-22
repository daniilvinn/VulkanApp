#pragma once

#include <vulkan/vulkan.h>

class GPUController
{
public:
	void ExecuteCommandBuffer(VkCommandBuffer cmdbuff);
};

