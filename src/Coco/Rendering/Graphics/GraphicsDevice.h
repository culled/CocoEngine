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

		/// @brief If true, the anisotropic sampling will be enabled on the device if it supports it
		bool EnableAnisotropicSampling;

		/// @brief If true, depth clamping will be enabled on the device if it supports it
		bool EnableDepthClamping;

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

		/// @brief Gets this device's features
		/// @return This device's features
		virtual const GraphicsDeviceFeatures& GetFeatures() const = 0;

		/// @brief Waits until the device has finished all queued work
		virtual void WaitForIdle() const = 0;

		/// @brief Gets the alignment for the given type of data
		/// @param type The type of data
		/// @return The alignment for the type of data
		virtual uint8 GetDataTypeAlignment(BufferDataType type) const = 0;

		/// @brief Aligns an offset to align with the alignment of the given type of data
		/// @param type The data type
		/// @param offset The offset to modify
		virtual void AlignOffset(BufferDataType type, uint64& offset) const = 0;

		/// @brief Creates a GraphicsPresenter
		/// @return The created presenter
		virtual Ref<GraphicsPresenter> CreatePresenter() = 0;

		/// @brief Creates a Buffer
		/// @param size The size of the buffer, in bytes
		/// @param usageFlags The usage flags
		/// @param bind If true, the buffer will be bound after it's created
		/// @return The created buffer
		virtual Ref<Buffer> CreateBuffer(uint64 size, BufferUsageFlags usageFlags, bool bind) = 0;

		/// @brief Creates an Image
		/// @param description The image description
		/// @return The image
		virtual Ref<Image> CreateImage(const ImageDescription& description) = 0;

		/// @brief Creates an ImageSampler
		/// @param description The sampler description
		/// @return The image sampler
		virtual Ref<ImageSampler> CreateImageSampler(const ImageSamplerDescription& description) = 0;

		/// @brief Purges any resources that are no longer in use
		virtual void PurgeUnusedResources() = 0;

		/// @brief Performs any tasks that need to occur before a new frame is drawn
		virtual void ResetForNewFrame() = 0;
	};
}