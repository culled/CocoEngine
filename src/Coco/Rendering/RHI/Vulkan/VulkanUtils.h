//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_VULKANUTILS_H
#define COCOENGINE_VULKANUTILS_H


#include "VulkanDescriptorSetLayoutBuilder.h"
#include "Coco/Core/Types/CoreTypes.h"
#include "Coco/Core/Types/Version.h"
#include "VulkanExceptions.h"
#include "Coco/Rendering/Graphics/GraphicsPlatformTypes.h"
#include "Coco/Rendering/Graphics/Resources/ImageTypes.h"
#include "Coco/Rendering/Graphics/Resources/GraphicsSurface.h"
#include "Coco/Rendering/ShaderTypes.h"

#include "Coco/Rendering/Graphics/VertexDataTypes.h"
#include "Coco/Rendering/Graphics/Resources/ImageSamplerTypes.h"

#include "Coco/Rendering/RHI/Vulkan/VulkanIncludes.h"

namespace slang
{
    enum class BindingType : unsigned int;
}

namespace Coco
{
    class VulkanUtils
    {
    public:
        static uint32 ToVkVersion(const Version& version);
        static Version ToVersion(uint32 vulkanVersion);
        static GraphicsDeviceType ToGraphicsDeviceType(VkPhysicalDeviceType deviceType);
        static MSAASamples ToMSAASamples(VkSampleCountFlags sampleCountFlags);
        static VSyncMode ToVSyncMode(VkPresentModeKHR presentMode);
        static ImagePixelFormat ToImagePixelFormat(VkFormat format);
        static VkFormat ToVkFormat(ImagePixelFormat format, ImageColorSpace colorSpace);
        static ImageColorSpace ToImageColorSpace(VkFormat format);
        static VkImageViewType ToVkImageViewType(uint32 height, uint32 depth, uint32 layers);
        static VkImageType ToVkImageType(uint32 height, uint32 depth);
        static VkImageUsageFlags ToVkImageUsageFlags(ImageUsageFlags usageFlags, ImageAttachmentType attachmentType);
        static VkSampleCountFlagBits ToVkSampleFlags(MSAASamples msaaSamples);
        static VkImageLayout ToVkImageLayout(ImageAttachmentType attachmentType);
        static VkClearValue ToVkClearValue(const RenderTargetClearValue& value, ImageAttachmentType attachmentType, ImagePixelType pixelType);
        static VkImageAspectFlags ToVkImageAspectFlags(ImageAttachmentType attachmentType);
        static VkDescriptorType ToVkDescriptorType(slang::BindingType bindingType);

        /// @brief Converts a FillMode to a VkPolygonMode
        /// @param fillMode The FillMode
        /// @return The VkPolygonMode
        static VkPolygonMode ToVkPolygonMode(PolygonFillMode fillMode) noexcept;

        /// @brief Converts a CullMode to VkCullModeFlags
        /// @param cullMode The CullMode
        /// @return The VkCullModeFlags
        static VkCullModeFlags ToVkCullModeFlags(CullMode cullMode) noexcept;

        /// @brief Converts a TriangleWindingMode to a VkFrontFace
        /// @param windingMode The TriangleWindingMode
        /// @return The VkFrontFace
        static VkFrontFace ToVkFrontFace(TriangleWindingMode windingMode) noexcept;

        /// @brief Converts a DepthTestingMode to a VkCompareOp
        /// @param depthMode The DepthTestingMode
        /// @return The VkCompareOp
        static VkCompareOp ToVkCompareOp(DepthTestingMode depthMode) noexcept;

        /// @brief Converts a BlendFactorMode to a VkBlendFactor
        /// @param mode The BlendFactorMode
        /// @return The VkBlendFactor
        static VkBlendFactor ToVkBlendFactor(BlendFactorMode mode) noexcept;

        /// @brief Converts a BlendOperation to a VkBlendOp
        /// @param op The BlendOperation
        /// @return The VkBlendOp
        static VkBlendOp ToVkBlendOp(BlendOperation op) noexcept;

        /// @brief Converts a TopologyMode to a VkPrimitiveTopology
        /// @param mode The TopologyMode
        /// @return The VkPrimitiveTopology
        static VkPrimitiveTopology ToVkPrimitiveTopology(MeshTopologyMode mode) noexcept;

        static VkFormat ToVkFormat(VertexChannel channel);

        /// @brief Converts BufferUsageFlags to VkBufferUsageFlags
        /// @param usageFlags The BufferUsageFlags
        /// @return The VkBufferUsageFlags
        static VkBufferUsageFlags ToVkBufferUsageFlags(BufferUsageFlags usageFlags) noexcept;

        /// @brief Converts an ImageFilterMode to a VkFilter
        /// @param mode The ImageFilterMode
        /// @return The VkFilter
        static VkFilter ToVkFilter(ImageFilterMode mode) noexcept;

        /// @brief Converts an ImageRepeatMode to a VkSamplerAddressMode
        /// @param repeatMode The ImageRepeatMode
        /// @return The VkSamplerAddressMode
        static VkSamplerAddressMode ToVkSamplerAddressMode(ImageRepeatMode repeatMode) noexcept;

        /// @brief Converts a MipMapFilterMode to a VkSamplerMipmapMode
        /// @param mode The MipMapFilterMode
        /// @return The VkSamplerMipmapMode
        static VkSamplerMipmapMode ToVkSamplerMipmapMode(MipMapFilterMode mode) noexcept;

        static VkShaderStageFlags ToVkShaderStageFlags(ShaderStageType shaderStage);

        static ShaderStageFlags ToShaderStageFlags(VkShaderStageFlags stageFlags);
    };
} // Coco

#define AssertVkSuccess(Expression){ VkResult r = Expression; if(r != VK_SUCCESS) { throw Coco::VulkanOperationException(r); } }
#endif //COCOENGINE_VULKANUTILS_H