#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/Matrix.h>
#include "Resources/ImageTypes.h"
#include "Resources/ImageSamplerTypes.h"
#include "Resources/BufferTypes.h"
#include "GraphicsPlatformTypes.h"

namespace Coco
{
	class Resource;
}

namespace Coco::Logging
{
	class Logger;
}

namespace Coco::Rendering
{
	class GraphicsDevice;
	class RenderingService;
	class Image;
	class Buffer;
	class ImageSampler;
	class GraphicsPresenter;

	/// @brief A platform that interfaces with low-level rendering hardware
	class COCOAPI GraphicsPlatform
	{
	public:
		/// @brief If true, this platform supports presenting images to windows
		const bool SupportsPresentation;

		/// @brief A pointer to the rendering service
		RenderingService* const RenderService;

	protected:
		GraphicsPlatform(RenderingService* renderingService, const GraphicsPlatformCreationParameters& creationParams) noexcept;

	public:
		virtual ~GraphicsPlatform() = default;

		GraphicsPlatform(const GraphicsPlatform&) = delete;
		GraphicsPlatform(GraphicsPlatform&&) = delete;

		GraphicsPlatform& operator=(const GraphicsPlatform&) = delete;
		GraphicsPlatform& operator=(GraphicsPlatform&&) = delete;

		/// @brief Creates a graphics platform
		/// @param renderingService The rendering service
		/// @param creationParams Platform creation parameters
		/// @return A created graphics platform
		static ManagedRef<GraphicsPlatform> CreatePlatform(RenderingService* renderingService, const GraphicsPlatformCreationParameters& creationParams);

		/// @brief Gets this graphics platform's logger
		/// @return This graphics platform's logger
		virtual Logging::Logger* GetLogger() noexcept = 0;

		/// @brief Gets the render hardware interface that this platform uses
		/// @return The render hardware interface that this platform uses
		virtual RenderingRHI GetRHI() const noexcept = 0;

		/// @brief Gets the graphics device that is used for rendering
		/// @return The graphics device that is used for rendering
		virtual GraphicsDevice* GetDevice() noexcept = 0;

		/// @brief Resets the graphics device
		//virtual void ResetDevice() = 0;

		/// @brief Creates a graphics presenter
		/// @return A graphics presenter
		virtual Ref<GraphicsPresenter> CreatePresenter(const string& name) = 0;

		/// @brief Creates a data buffer
		/// @param size The size of the buffer (in bytes)
		/// @param usageFlags Flags for how the buffer will be used
		/// @param bindOnCreate If true, the buffer will be bound after it is created
		/// @return The created buffer
		virtual Ref<Buffer> CreateBuffer(const string& name, uint64_t size, BufferUsageFlags usageFlags, bool bindOnCreate) = 0;

		/// @brief Creates an image
		/// @param description The description for the image
		/// @return The created image
		virtual Ref<Image> CreateImage(const string& name, const ImageDescription& description) = 0;

		/// @brief Creates an image sampler
		/// @param filterMode The filter mode
		/// @param repeatMode The repeat mode
		/// @param maxAnisotropy The maximum anisotropy
		/// @return The created image sampler
		virtual Ref<ImageSampler> CreateImageSampler(const string& name, const ImageSamplerProperties& properties) = 0;

		virtual void PurgeResource(const Ref<Resource>& resource, bool forcePurge = false);
	};
}
