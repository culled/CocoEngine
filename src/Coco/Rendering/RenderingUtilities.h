#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
    /// @brief Various rendering utilities
    class COCOAPI RenderingUtilities
    {
    public:
        /// @brief Pads out an offset to align with a given alignment
        /// @param originalOffset The original offset
        /// @param alignment The desired alignment
        /// @return An adjusted offset that respects the given alignment
        static constexpr uint64_t GetOffsetForAlignment(uint64_t originalOffset, uint64_t alignment) noexcept
        {
            return alignment > 0 ? (originalOffset + alignment - 1) & ~(alignment - 1) : alignment;
        }
    };
}