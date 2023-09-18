#pragma once

#include "../Renderpch.h"
#include "GraphicsDeviceTypes.h"
#include "GraphicsPresenter.h"
#include <Coco/Core/Types/Version.h>
#include "Buffer.h"

namespace Coco::Rendering
{
	/// @brief Creation parameters for a GraphicsDevice
	struct GraphicsDeviceCreateParams
	{
		/// @brief The ideal type of rendering device to pick
		GraphicsDeviceType PreferredDeviceType;

		/// @brief If true, the device will be able to present images
		bool SupportsPresentation;

		/// @brief If true, the device will be required to support graphics operations
		bool RequireGraphicsCapability;

		/// @brief If true, the device will be required to support compute operations
		bool RequireComputeCapability;

		/// @brief If true, the device will be required to support memory transfer operations
		bool RequireTransferCapability;

		GraphicsDeviceCreateParams();
	};

	/// @brief A device that can perform graphics operations
	class GraphicsDevice
	{
	public:
		virtual ~GraphicsDevice() = default;

		/// @brief Pads out an offset to align with a given alignment
		/// @param originalOffset The original offset
		/// @param alignment The desired alignment
		/// @return An adjusted offset that respects the given alignment
		static constexpr uint64 GetOffsetForAlignment(uint64 originalOffset, uint64 alignment) noexcept
		{
			return alignment > 0 ? (originalOffset + alignment - 1) & ~(alignment - 1) : alignment;
		}

		/// @brief Gets the name of this device
		/// @return This device's name
		virtual const char* GetName() const = 0;

		/// @brief Gets the type of this device
		/// @return This device's type
		virtual GraphicsDeviceType GetDeviceType() const = 0;

		/// @brief Gets the driver version of this device
		/// @return This device's driver version
		virtual Version GetDriverVersion() const = 0;

		/// @brief Gets the api version of this device
		/// @return This device's api version
		virtual Version GetAPIVersion() const = 0;

		/// @brief Gets this device's memory features
		/// @return This device's memory features
		virtual const GraphicsDeviceMemoryFeatures& GetMemoryFeatures() const = 0;

		/// @brief Waits until the device has finished all queued work
		virtual void WaitForIdle() const = 0;

		virtual uint32 GetMinimumBufferAlignment() const = 0;
		virtual uint8 GetDataTypeAlignment(BufferDataType type) const = 0;
		virtual void AlignOffset(BufferDataType type, uint64& offset) const = 0;

		/// @brief Creates a GraphicsPresenter
		/// @return The created presenter
		virtual Ref<GraphicsPresenter> CreatePresenter() = 0;

		virtual Ref<Buffer> CreateBuffer(uint64 size, BufferUsageFlags usageFlags, bool bind) = 0;
		virtual Ref<Image> CreateImage(const ImageDescription& description) = 0;
		virtual Ref<ImageSampler> CreateImageSampler(const ImageSamplerDescription& description) = 0;

		virtual void PurgeUnusedResources() = 0;
	};
}