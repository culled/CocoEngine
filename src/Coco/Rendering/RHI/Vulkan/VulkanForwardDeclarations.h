//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_VULKANFORWARDDECLARATIONS_H
#define COCOENGINE_VULKANFORWARDDECLARATIONS_H

// Common Vulkan types
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;
typedef struct VkInstance_T* VkInstance;
typedef struct VkDebugUtilsMessengerEXT_T* VkDebugUtilsMessengerEXT;
struct VkAllocationCallbacks;
struct VkDebugUtilsMessengerCreateInfoEXT;
typedef struct VkDevice_T* VkDevice;
typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkQueue_T* VkQueue;
struct VkSurfaceFormatKHR;
struct VkSurfaceCapabilitiesKHR;
struct VkExtent2D;
typedef struct VkSwapchainKHR_T* VkSwapchainKHR;
typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;
typedef struct VkSemaphore_T* VkSemaphore;
typedef struct VkFence_T* VkFence;
typedef struct VkCommandPool_T* VkCommandPool;
typedef struct VkCommandBuffer_T* VkCommandBuffer;
typedef struct VkPipeline_T* VkPipeline;
typedef struct VkShaderModule_T* VkShaderModule;
typedef struct VkDescriptorSetLayout_T* VkDescriptorSetLayout;
struct VkPushConstantRange;
typedef struct VkPipelineLayout_T* VkPipelineLayout;
struct VkVertexInputBindingDescription;
struct VkVertexInputAttributeDescription;
struct VkPipelineVertexInputStateCreateInfo;
struct VkPipelineShaderStageCreateInfo;
typedef struct VkBuffer_T* VkBuffer;
typedef struct VkDescriptorPool_T* VkDescriptorPool;
typedef struct VkDescriptorSet_T* VkDescriptorSet;
struct VkDescriptorSetLayoutBinding;

// Vulkan Memory Allocator types
typedef struct VmaAllocator_T* VmaAllocator;
typedef struct VmaAllocation_T* VmaAllocation;
struct VmaAllocationInfo;

#endif //COCOENGINE_VULKANFORWARDDECLARATIONS_H