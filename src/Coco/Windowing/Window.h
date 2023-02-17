#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Optional.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/Vector.h>

namespace Coco::Windowing
{
	class WindowingService;

	/// <summary>
	/// Parameters for creating a window
	/// </summary>
	struct COCOAPI WindowCreateParameters
	{
		string Title;
		SizeInt InitialSize;
		bool IsResizable = true;
		Optional<Vector2Int> InitialPosition;

		WindowCreateParameters(const string& title, const SizeInt initialSize, bool isResizable = true) :
			Title(title), InitialSize(initialSize), IsResizable(isResizable)
		{}
	};

	/// <summary>
	/// A GUI window
	/// </summary>
	class COCOAPI Window
	{
	protected:
		WindowingService* WindowingService;

	protected:
		Window(Windowing::WindowingService* windowingService);

	public:
		virtual ~Window();

		/// <summary>
		/// Gets the platform-specific ID for this window
		/// </summary>
		/// <returns>The ID for this window</returns>
		virtual void* GetID() const = 0;

		/// <summary>
		/// Shows this window
		/// </summary>
		virtual void Show() = 0;

		/// <summary>
		/// Hides this window
		/// </summary>
		virtual void Hide() = 0;

		/// <summary>
		/// Requests this window to close
		/// </summary>
		/// <returns>True if this window will close</returns>
		bool Close();
	};
}