#pragma once

#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/String.h>
#include <Coco/Core/Types/Version.h>
#include "BufferTypes.h"
#include "ImageTypes.h"
#include "ImageSamplerTypes.h"
#include "GraphicsDeviceTypes.h"
#include "GraphicsResource.h"

namespace Coco::Rendering
{
	class Presenter;
	class RenderFrame;
	class Buffer;
	class Image;
	class ImageSampler;

	/// @brief A device that can perform rendering operations
	class GraphicsDevice
	{
	friend class RenderService;

	public:
		virtual ~GraphicsDevice() = default;

		/// @brief Gets this device's name
		/// @return The name
		virtual const string& GetName() const = 0;

		/// @brief Gets this device's type
		/// @return The type
		virtual GraphicsDeviceType GetDeviceType() const = 0;

		/// @brief Gets this device's driver version
		/// @return The driver version
		virtual Version GetDriverVersion() const = 0;

		virtual const GraphicsDeviceFeatures& GetFeatures() const = 0;

		/// @brief Gets the alignment for the given type of data
		/// @param type The type of data
		/// @return The alignment for the type of data
		virtual uint32 GetDataTypeAlignment(BufferDataType type) const = 0;

		/// @brief Creates a Presenter resource
		/// @return The presenter
		virtual Ref<Presenter> CreatePresenter() = 0;

		virtual Ref<Buffer> CreateBuffer(uint64 size, BufferUsageFlags usageFlags) = 0;
		virtual Ref<Image> CreateImage(const ImageDescription& description) = 0;
		virtual Ref<ImageSampler> CreateImageSampler(const ImageSamplerDescription& description) = 0;

		/// @brief Attempts to release a GraphicsResource if it has one or less outside users. NOTE: The resource may become invalid if this succeeds
		/// @param resourceID The resource's ID
		virtual bool TryReleaseResource(const GraphicsResourceID& resourceID) = 0;

		/// @brief Gets the current RenderFrame
		/// @return The current RenderFrame
		virtual SharedRef<RenderFrame> GetCurrentRenderFrame() const = 0;

		/// @brief Blocks until the device is idle
		virtual void WaitForIdle() = 0;

	protected:
		/// @brief Starts a new RenderFrame
		/// @return The new RenderFrame
		virtual SharedRef<RenderFrame> StartNewRenderFrame() = 0;

		/// @brief Ends the current RenderFrame
		virtual void EndRenderFrame() = 0;
	};
}