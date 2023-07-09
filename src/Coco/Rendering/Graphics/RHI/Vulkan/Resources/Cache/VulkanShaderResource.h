#pragma once

#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>
#include <Coco/Core/Resources/CachedResource.h>

#include "../../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class VulkanShader;
	class VulkanDescriptorPool;

	/// @brief A cached resource for a Vulkan shader
	class VulkanShaderResource : public GraphicsResource<GraphicsDeviceVulkan, RenderingResource>, public CachedResource
	{
	private:
		static constexpr uint _maxSets = 512;

		Ref<VulkanDescriptorPool> _pool;

	public:
		VulkanShaderResource(ResourceID id, const string& name, const VulkanShader& shader);
		~VulkanShaderResource();

		DefineResourceType(VulkanShaderResource)

		bool IsValid() const noexcept final { return true; }
		bool NeedsUpdate() const noexcept final { return false; }

		/// @brief Determines if this resource needs to be updated to reflect the given shader
		/// @param shader The shader
		/// @return True if this resource should be updated
		bool NeedsUpdate(const VulkanShader& shader) const noexcept;

		/// @brief Updates this cached resource
		/// @param shader The Vulkan shader to update with
		void Update(const VulkanShader& shader);

		/// @brief Gets the descriptor pool
		/// @return The descriptor pool
		Ref<VulkanDescriptorPool> GetPool() { return _pool; }

	private:
		/// @brief Destroys the descriptor pool
		void DestroyPool();
	};
}