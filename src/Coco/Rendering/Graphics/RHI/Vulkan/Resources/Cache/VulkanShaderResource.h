#pragma once

#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>
#include <Coco/Core/Resources/CachedResource.h>

#include "../../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class VulkanShader;
	class VulkanDescriptorPool;

	class VulkanShaderResource : public GraphicsResource<GraphicsDeviceVulkan, RenderingResource>, public CachedResource
	{
		static constexpr uint _maxSets = 512;
		Ref<VulkanDescriptorPool> _pool;

	public:
		VulkanShaderResource(ResourceID id, const string& name, uint64_t lifetime, const VulkanShader* shader);
		~VulkanShaderResource();

		DefineResourceType(VulkanShaderResource)

		bool IsValid() const noexcept final { return true; }
		bool NeedsUpdate() const noexcept final { return false; }
		bool NeedsUpdate(const VulkanShader* shader) const noexcept;

		/// @brief Updates this cached resource
		void Update(VulkanShader* shader);

		Ref<VulkanDescriptorPool> GetPool() { return _pool; }

	private:
		void DestroyPool();
	};
}