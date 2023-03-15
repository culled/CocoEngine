#include "VulkanUtilities.h"

namespace Coco::Rendering
{
	Version ToVersion(uint32_t version) noexcept
	{
		return Version(VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), VK_VERSION_PATCH(version));
	}

	uint32_t ToVkVersion(const Version& version) noexcept
	{
		return VK_MAKE_VERSION(version.Major, version.Minor, version.Patch);
	}

	VkBufferUsageFlags ToVkBufferUsageFlags(BufferUsageFlags usageFlags) noexcept
	{
		VkBufferUsageFlags flags = 0;

		if ((usageFlags & BufferUsageFlags::TransferSource) == BufferUsageFlags::TransferSource)
			flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		if ((usageFlags & BufferUsageFlags::TransferDestination) == BufferUsageFlags::TransferDestination)
			flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		if ((usageFlags & BufferUsageFlags::UniformTexel) == BufferUsageFlags::UniformTexel)
			flags |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;

		if ((usageFlags & BufferUsageFlags::StorageTexel) == BufferUsageFlags::StorageTexel)
			flags |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;

		if ((usageFlags & BufferUsageFlags::Uniform) == BufferUsageFlags::Uniform)
			flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		if ((usageFlags & BufferUsageFlags::Storage) == BufferUsageFlags::Storage)
			flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

		if ((usageFlags & BufferUsageFlags::Index) == BufferUsageFlags::Index)
			flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

		if ((usageFlags & BufferUsageFlags::Vertex) == BufferUsageFlags::Vertex)
			flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

		if ((usageFlags & BufferUsageFlags::Indirect) == BufferUsageFlags::Indirect)
			flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		
		return flags;
	}

	VkImageUsageFlags ToVkImageUsageFlags(ImageUsageFlags usageFlags) noexcept
	{
		VkImageUsageFlags flags = 0;

		if ((usageFlags & ImageUsageFlags::TransferSource) == ImageUsageFlags::TransferSource)
			flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

		if ((usageFlags & ImageUsageFlags::TransferDestination) == ImageUsageFlags::TransferDestination)
			flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		if ((usageFlags & ImageUsageFlags::Sampled) == ImageUsageFlags::Sampled)
			flags |= VK_IMAGE_USAGE_SAMPLED_BIT;

		if ((usageFlags & ImageUsageFlags::RenderTarget) == ImageUsageFlags::RenderTarget)
			flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		return flags;
	}
}