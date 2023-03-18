#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
    class COCOAPI RenderingUtilities
    {
    public:
        /// <summary>
        /// Pads out an offset to align with a given alignment
        /// </summary>
        /// <param name="originalOffset">The original offset</param>
        /// <param name="alignment">The desired alignment</param>
        /// <returns>An adjusted offset that respects the given alignment</returns>
        static constexpr uint64_t GetOffsetForAlignment(uint64_t originalOffset, uint64_t alignment) noexcept
        {
            return alignment > 0 ? (originalOffset + alignment - 1) & ~(alignment - 1) : alignment;
        }
    };
}