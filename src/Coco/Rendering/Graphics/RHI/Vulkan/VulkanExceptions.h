#pragma once

#include <Coco/Core/Types/CoreExceptions.h>

enum VkResult;

namespace Coco::Rendering::Vulkan
{
	class VulkanException :
		public Exception
	{
	public:
		VulkanException(const string& message);
	};

	class VulkanOperationException :
		public VulkanException
	{
	public:
		VulkanOperationException(VkResult result, const string& message);

		VkResult GetResult() const { return _result; }

	private:
		VkResult _result;
	};
}

