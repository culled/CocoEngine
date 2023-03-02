#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Logging/Logger.h>

#include "GraphicsPlatformTypes.h"
#include "GraphicsDevice.h"
#include "GraphicsPresenter.h"
#include "RenderContext.h"
#include "RenderView.h"
#include "Image.h"
#include <Coco/Rendering/Pipeline/RenderPipeline.h>

namespace Coco::Rendering
{
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

	class RenderingService;

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

	public:
		virtual ~GraphicsPlatform() = default;

		/// <summary>
		/// Converts an RenderingRHI to a string
		/// </summary>
		/// <param name="rhi">The render hardware interface</param>
		/// <returns>The string equivalent of the RHI</returns>
		static string PlatformRHIToString(RenderingRHI rhi);

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
		virtual Logging::Logger* GetLogger() const = 0;

		/// <summary>
		/// Gets the render hardware interface that this platform uses
		/// </summary>
		/// <returns>The render hardware interface that this platform uses</returns>
		virtual RenderingRHI GetRHI() const = 0;

		/// <summary>
		/// Gets the graphics device that is used for rendering
		/// </summary>
		/// <returns>The graphics device that is used for rendering</returns>
		virtual GraphicsDevice* GetDevice() const = 0;

		/// <summary>
		/// Resets the graphics device
		/// </summary>
		virtual void ResetDevice() = 0;

		/// <summary>
		/// Creates a graphics presenter
		/// </summary>
		/// <returns>A graphics presenter</returns>
		virtual Managed<GraphicsPresenter> CreatePresenter() = 0;

	protected:
		GraphicsPlatform(RenderingService* renderingService, const GraphicsPlatformCreationParameters& creationParams);
	};
}
