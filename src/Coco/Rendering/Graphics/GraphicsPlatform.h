#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Logging/Logger.h>

#include "GraphicsPlatformTypes.h"
#include "GraphicsDevice.h"
#include "GraphicsPresenter.h"
#include "Buffer.h"
#include "Image.h"
#include "ImageSampler.h"
#include <Coco/Core/Types/Matrix.h>

namespace Coco::Rendering
{
	class RenderingService;

	/// <summary>
	/// Parameters for creating a graphics platform
	/// </summary>
	struct COCOAPI GraphicsPlatformCreationParameters
	{
		/// <summary>
		/// The name of the application
		/// </summary>
		string ApplicationName;

		/// <summary>
		/// The type of render hardware interface to use
		/// </summary>
		RenderingRHI RHI;

		/// <summary>
		/// Parameters for creating the graphics device
		/// </summary>
		GraphicsDeviceCreationParameters DeviceCreateParams = {};

		GraphicsPlatformCreationParameters(const string& applicationName, RenderingRHI rhi) : ApplicationName(applicationName), RHI(rhi) {}
	};

	/// <summary>
	/// A generic exception for a GraphicsPlatform
	/// </summary>
	using GraphicsPlatformException = Exception;

	/// <summary>
	/// An exception thrown when a GraphicsPlatform cannot be created
	/// </summary>
	using GraphicsPlatformCreateException = GraphicsPlatformException;

	/// <summary>
	/// A platform that interfaces with low-level rendering hardware
	/// </summary>
	class COCOAPI GraphicsPlatform
	{
	public:
		/// <summary>
		/// If true, this platform supports presenting images to windows
		/// </summary>
		const bool SupportsPresentation;

	protected:
		/// <summary>
		/// The render service
		/// </summary>
		RenderingService* RenderService;

	protected:
		GraphicsPlatform(RenderingService* renderingService, const GraphicsPlatformCreationParameters& creationParams) noexcept;

	public:
		virtual ~GraphicsPlatform() = default;

		GraphicsPlatform(const GraphicsPlatform&) = delete;
		GraphicsPlatform(GraphicsPlatform&&) = delete;

		GraphicsPlatform& operator=(const GraphicsPlatform&) = delete;
		GraphicsPlatform& operator=(GraphicsPlatform&&) = delete;

		/// <summary>
		/// Converts an RenderingRHI to a string
		/// </summary>
		/// <param name="rhi">The render hardware interface</param>
		/// <returns>The string equivalent of the RHI</returns>
		static constexpr string PlatformRHIToString(RenderingRHI rhi) noexcept
		{
			switch (rhi)
			{
			case RenderingRHI::Vulkan:
				return "Vulkan";
			case RenderingRHI::DirectX12:
				return "DirectX 12";
			case RenderingRHI::OpenGL:
				return "Open GL";
			default:
				return "Unknown";
			}
		}

		/// <summary>
		/// Creates a graphics platform
		/// </summary>
		/// <param name="renderingService">The service creating the platform</param>
		/// <param name="creationParams">Platform creation parameters</param>
		/// <returns>A created graphics platform</returns>
		static Managed<GraphicsPlatform> CreatePlatform(RenderingService* renderingService, const GraphicsPlatformCreationParameters& creationParams);

		/// <summary>
		/// Gets this graphics platform's logger
		/// </summary>
		/// <returns>This graphics platform's logger</returns>
		virtual Logging::Logger* GetLogger() const noexcept = 0;

		/// <summary>
		/// Gets the render hardware interface that this platform uses
		/// </summary>
		/// <returns>The render hardware interface that this platform uses</returns>
		virtual RenderingRHI GetRHI() const noexcept = 0;

		/// <summary>
		/// Gets the graphics device that is used for rendering
		/// </summary>
		/// <returns>The graphics device that is used for rendering</returns>
		virtual GraphicsDevice* GetDevice() const noexcept = 0;

		/// <summary>
		/// Resets the graphics device
		/// </summary>
		virtual void ResetDevice() = 0;

		/// <summary>
		/// Creates a graphics presenter
		/// </summary>
		/// <returns>A graphics presenter</returns>
		virtual Managed<GraphicsPresenter> CreatePresenter() = 0;

		/// <summary>
		/// Creates a data buffer
		/// </summary>
		/// <param name="size">The size of the buffer (in bytes)</param>
		/// <param name="usageFlags">Flags for how the buffer will be used</param>
		/// <param name="bindOnCreate">If true, the buffer will be bound after it is created</param>
		/// <returns>The created buffer</returns>
		virtual GraphicsResourceRef<Buffer> CreateBuffer(uint64_t size, BufferUsageFlags usageFlags, bool bindOnCreate) = 0;

		/// <summary>
		/// Creates an image
		/// </summary>
		/// <param name="description">The descriptor for the image</param>
		/// <returns>The created image</returns>
		virtual GraphicsResourceRef<Image> CreateImage(const ImageDescription& description) = 0;

		/// <summary>
		/// Creates a sampler for an image
		/// </summary>
		/// <param name="filterMode">The filter mode</param>
		/// <param name="repeatMode">The repeat mode</param>
		/// <param name="maxAnisotropy">The maximum anisotropy</param>
		/// <returns>The created image sampler</returns>
		virtual GraphicsResourceRef<ImageSampler> CreateImageSampler(FilterMode filterMode, RepeatMode repeatMode, uint maxAnisotropy) = 0;
	};
}
