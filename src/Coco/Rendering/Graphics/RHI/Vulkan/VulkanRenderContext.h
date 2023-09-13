#pragma once

#include <Coco/Core/Defines.h>
#include "../../RenderContext.h"
#include "VulkanGraphicsSemaphore.h"
#include "VulkanGraphicsFence.h"

namespace Coco::Rendering::Vulkan
{
    /// @brief Vulkan implementation of a RenderContext
    class VulkanRenderContext : public RenderContext
    {
    private:
        UniqueRef<VulkanGraphicsSemaphore> _imageAvailableSemaphore;
        UniqueRef<VulkanGraphicsSemaphore> _renderCompletedSemaphore;
        UniqueRef<VulkanGraphicsFence> _renderCompletedFence;

        int _backbufferIndex;

    public:
        VulkanRenderContext();
        ~VulkanRenderContext();

        void WaitForRenderingToComplete() final;

        GraphicsSemaphore* GetImageAvailableSemaphore() final { return _imageAvailableSemaphore.get(); }
        GraphicsSemaphore* GetRenderCompletedSemaphore() final { return _renderCompletedSemaphore.get(); }
        GraphicsFence* GetRenderCompletedFence() final { return _renderCompletedFence.get(); }

        /// @brief Sets the index of the backbuffer this context is rendering to when obtained via a VulkanGraphicsPresenter
        /// @param index The backbuffer index
        void SetBackbufferIndex(uint32 index);

        /// @brief Gets the index of the backbuffer this context is rendering to
        /// @return The backbuffer index, or -1 if this context was not obtained via a VulkanGraphicsPresenter
        int GetBackbufferIndex() const { return _backbufferIndex; }

    protected:
        void ResetImpl() final;
    };
}