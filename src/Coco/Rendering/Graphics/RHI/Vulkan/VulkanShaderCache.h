#pragma once
#include "../../ShaderCache.h"
#include "CachedResources/VulkanShaderVariant.h"

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
        /// @param stage The shader stage
        /// @param byteCode The bytecode for the shader module
        void Reflect(ShaderStage& stage, const std::vector<uint32>& byteCode);
    };
}