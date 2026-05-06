target_sources(Rendering PRIVATE
        VulkanGraphicsPlatform.h
        VulkanGraphicsPlatform.cpp
        VulkanRenderFrame.h
        VulkanRenderFrame.cpp
        VulkanUtils.h
        VulkanUtils.cpp
        VulkanForwardDeclarations.h
        VulkanRenderingExtensions.h
        VulkanExceptions.h
        VulkanExceptions.cpp
        VulkanGraphicsPlatformTypes.h
        VulkanQueue.h
        VulkanQueue.cpp
        VulkanResourceCache.h
        VulkanResourceCache.cpp
        VulkanUniformStorage.h
        VulkanUniformStorage.cpp
        VulkanShaderBufferInterface.h
        VulkanShaderBufferInterface.cpp
        VulkanDescriptorSetPool.h
        VulkanDescriptorSetPool.cpp
        VulkanDescriptorSetLayout.h
        VulkanDescriptorSetLayout.cpp
        VulkanIncludes.h
        VulkanStagingBuffer.h
        VulkanStagingBuffer.cpp
        VulkanDescriptorSetLayoutBuilder.h
        VulkanDescriptorSetLayoutBuilder.cpp
        VulkanPipelineLayoutBuilder.h
        VulkanPipelineLayoutBuilder.cpp
        VulkanPipelineLayout.h
        VulkanPipelineLayout.cpp
        VulkanCommandPool.h
        VulkanCommandPool.cpp
        Resources/VulkanRenderContext.h
        Resources/VulkanRenderContext.cpp
        Resources/VulkanImage.h
        Resources/VulkanImage.cpp
        Resources/VulkanShaderProgram.h
        Resources/VulkanShaderProgram.cpp
        Resources/VulkanBuffer.h
        Resources/VulkanBuffer.cpp
        Resources/VulkanGraphicsSurface.h
        Resources/VulkanGraphicsSurface.cpp
        Resources/VulkanGraphicsSemaphore.h
        Resources/VulkanGraphicsSemaphore.cpp
        Resources/VulkanGraphicsFence.h
        Resources/VulkanGraphicsFence.cpp
        Resources/VulkanImageSampler.h
        Resources/VulkanImageSampler.cpp
        CachedResources/VulkanPipeline.h
        CachedResources/VulkanPipeline.cpp
        Vendor/vma.cpp
)

# TODO: more platforms
if(UNIX)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_XLIB_KHR)
endif()

FetchContent_Declare(
        volk
        GIT_REPOSITORY https://github.com/zeux/volk.git
        GIT_TAG 1.4.304
)

FetchContent_MakeAvailable(volk)

target_include_directories(Rendering PRIVATE
        ${volk_SOURCE_DIR}
)

target_link_libraries(Rendering PRIVATE volk)

FetchContent_Declare(
        VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG v3.3.0
)

FetchContent_MakeAvailable(VulkanMemoryAllocator)

target_include_directories(Rendering PRIVATE
        ${VulkanMemoryAllocator_SOURCE_DIR}/include
)

target_link_libraries(Rendering PRIVATE VulkanMemoryAllocator)