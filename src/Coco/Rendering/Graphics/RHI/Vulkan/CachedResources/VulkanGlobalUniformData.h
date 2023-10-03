#pragma once
#include "CachedVulkanResource.h"
#include <Coco/Core/Types/Refs.h>
#include "../../../ShaderUniformLayout.h"
#include "../VulkanDescriptorSetLayout.h"
#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;
    class VulkanBuffer;

    /// @brief Cached global uniform data for a VulkanRenderContext
    class VulkanGlobalUniformData :
        public CachedVulkanResource
    {
    private:
        GlobalShaderUniformLayout _uniformLayout;
        VulkanDescriptorSetLayout _descriptorSetLayout;
        VkDescriptorPool _pool;
        VkDescriptorSet _set;

        ManagedRef<VulkanBuffer> _uniformBuffer;
        std::unordered_map<ShaderUniformData::UniformKey, ManagedRef<VulkanBuffer>> _globalBuffers;

    public:
        VulkanGlobalUniformData(const GlobalShaderUniformLayout& layout, const VulkanDescriptorSetLayout* descriptorSetLayout);
        ~VulkanGlobalUniformData();

        /// @brief Makes a key for the given layout
        /// @param layout The layout
        /// @return A key
        static GraphicsDeviceResourceID MakeKey(const GlobalShaderUniformLayout& layout);

        /// @brief Sets data for a buffer uniform
        /// @param key The uniform key
        /// @param dataOffset The offset into the buffer to start copying data
        /// @param data The data
        /// @param dataSize The size of the data
        void SetBufferUniformData(ShaderUniformData::UniformKey key, uint64 dataOffset, const void* data, uint64 dataSize);

        /// @brief Prepares global uniform data
        /// @param uniformData The uniform data
        /// @return The descriptor set for the global data, or nullptr if there was no uniform data
        VkDescriptorSet PrepareData(const ShaderUniformData& uniformData);

        /// @brief Gets the global descriptor set layout
        /// @return The global descriptor set layout
        const VulkanDescriptorSetLayout& GetDescriptorSetLayout() const { return _descriptorSetLayout; }

    private:
        /// @brief Creates the global descriptor set pool
        void CreateDescriptorSetPool();

        /// @brief Allocates the global descriptor set
        void AllocateDescriptorSet();

        /// @brief Updates the global descriptor set
        /// @param uniformData The uniform data
        /// @return True if the global descriptor set was updated successfully
        bool WriteDescriptorSet(const ShaderUniformData& uniformData);

        /// @brief Creates a shader global uniform buffer
        /// @param uniform The buffer uniform
        void CreateGlobalBuffer(const ShaderBufferUniform& uniform);
    };
}