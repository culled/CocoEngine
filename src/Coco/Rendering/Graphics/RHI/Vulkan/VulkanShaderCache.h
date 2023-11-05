#pragma once
#include "../../ShaderCache.h"
#include "CachedResources/VulkanShaderVariant.h"

namespace spirv_cross
{
    class Compiler;
    struct Resource;
}

namespace Coco::Rendering::Vulkan
{
    /// @brief A cache for VulkanShaderVariant
    class VulkanShaderCache :
        public ShaderCache
    {
    private:
        static const string _sCacheDirectory;

        double _lastPurgeTime;
        std::unordered_map<GraphicsDeviceResourceID, VulkanShaderVariant> _shaders;

    public:
        VulkanShaderCache();
        ~VulkanShaderCache();

        // Inherited via ShaderCache
        ShaderVariant LoadVariant(const std::unordered_map<ShaderStageType, string>& stageFiles) override;

        /// @brief Gets/creates a VulkanShaderVariant 
        /// @param variantData The shader data
        /// @return The shader
        VulkanShaderVariant& GetOrCreateShader(const ShaderVariantData& variantData);

        /// @brief Resets this cache for a new frame
        void ResetForNextFrame();

        /// @brief Purges all stale resources
        void PurgeStaleResources();

        /// @brief Gets/compiles the binary shader data for a shader stage
        /// @param stage The shader stage
        /// @return The shader module bytecode
        std::vector<uint32> CompileOrGetShaderStageBinary(ShaderStage& stage);

        /// @brief Performs reflection on a shader stage
        /// @param variant The variant to reflect with
        /// @param stage The shader stage
        /// @param byteCode The bytecode for the shader module
        void Reflect(ShaderVariant& variant, const ShaderStage& stage, const std::vector<uint32>& byteCode);

    private:
        void ReflectUniforms(
            ShaderUniformLayout& layout, 
            ShaderStageType stage, 
            const spirv_cross::Compiler& compiler, 
            const spirv_cross::Resource& bufferResource);

        void ReflectUniformBlock(
            GlobalShaderUniformLayout& layout, 
            ShaderStageType stage, 
            const spirv_cross::Compiler& compiler, 
            const spirv_cross::Resource& bufferResource);

        void ReflectTexture(
            ShaderVariant& variant,
            ShaderStageType stage,
            const spirv_cross::Compiler& compiler,
            const spirv_cross::Resource& imageResource);
    };
}