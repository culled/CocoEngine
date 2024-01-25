#pragma once

#include "CachedVulkanResource.h"
#include <Coco/Core/Types/Refs.h>
#include "../../../../Shader.h"

#include "../VulkanIncludes.h"
#include <spirv_cross/spirv_common.hpp>
#include <shaderc/shaderc.hpp>

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

    /// @brief A Vulkan subshader stage
    struct VulkanShaderStage : public ShaderStage
    {
        /// @brief The Vulkan shader module
        VkShaderModule ShaderModule;

        VulkanShaderStage(const ShaderStage& stage);
    };

    class VulkanShader :
        public CachedVulkanResource
    {
    public:
        static const string CacheDirectory;

    public:
        VulkanShader(uint64 id, VulkanGraphicsDevice& device, const SharedRef<Shader>& shader);
        ~VulkanShader();

        static uint64 MakeKey(const Shader& shader);

        // Inherited via CachedVulkanResource
        void Use() override;
        bool IsStale(double staleThreshold) const override;

        SharedRef<Shader> GetBaseShader() const { return _shader.expired() ? nullptr : _shader.lock(); }

        /// @brief Determines if this shader has a descriptor set layout for the given scope
        /// @param scope The uniform scope
        /// @return True if this shader has a layout for the given scope
        bool HasScope(UniformScope scope) const;

        const ShaderUniformLayout& GetUniformLayout(UniformScope scope) const;

        /// @brief Gets the push constant ranges for the draw uniforms
        /// @return The push constant ranges
        std::span<const VkPushConstantRange> GetPushConstantRanges() const { return _pushConstantRanges; }
        std::span<const VulkanShaderStage> GetStages() const { return _stages; }
        uint64 GetVersion() const { return _version; }

    private:
        VulkanGraphicsDevice& _device;
        WeakSharedRef<Shader> _shader;
        ResourceVersion _version;
        std::vector<VulkanShaderStage> _stages;
        std::vector<VkPushConstantRange> _pushConstantRanges;

    private:
        static shaderc_shader_kind ShaderStageToShaderC(ShaderStageType stage);
        static std::vector<uint32> CompileOrGetShaderStageBinary(ShaderStage& stage);

        void CreateShaderModules();
        void DestroyShaderModules();

        void CalculatePushConstantRanges();
    };
}