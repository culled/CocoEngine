#include "CPWinpch.h"
#include "Win32WindowExtensions.h"
#include <Coco/Core/Engine.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Windowing/WindowService.h>

#include "WindowsIncludes.h"

#ifdef COCO_RENDERING_VULKAN
#include <Coco/Rendering/Graphics/RHI/Vulkan/VulkanGraphicsPlatform.h>
#include <Coco/Rendering/Graphics/RHI/Vulkan/VulkanGraphicsPresenterSurface.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#endif

#include "Win32Window.h"
#include "Win32EnginePlatform.h"

//#define STRICT_TYPED_ITEMIDS
#include <shlobj.h>
//#include <objbase.h>      // For COM headers
//#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shlwapi.h>
//#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers
//#include <propvarutil.h>  // for PROPVAR-related functions
//#include <propkey.h>      // for the Property key APIs/datatypes
//#include <propidl.h>      // for the Property System APIs

#define AssertHResult(hr) { if(!SUCCEEDED(hr)) { throw std::exception("HRESULT error"); } }

namespace Coco::Platforms::Win32
{
	SharedRef<Rendering::GraphicsPresenterSurface> Win32WindowExtensions::CreateSurfaceForWindow(const char* renderRHIName, const Win32Window& window) const
	{
#ifdef COCO_RENDERING_VULKAN
		if (strcmp(renderRHIName, Rendering::Vulkan::VulkanGraphicsPlatform::sVulkanRHIName) == 0)
		{
			return CreateVulkanSurfaceForWindow(window);
		}
#endif

		string err = FormatString("Unsupported render RHI: {}", renderRHIName);
		throw std::exception(err.c_str());
	}

	string ShowDialog(CLSID dialogType, DWORD flags, HWND owner, const std::vector<std::pair<const char*, const char*>>& filters)
	{
		std::vector<COMDLG_FILTERSPEC> saveTypes;
		std::vector<std::pair<wstring, wstring>> saveTypesMem(filters.size());

		for (size_t i = 0; i < filters.size(); i++)
		{
			const auto& pair = filters.at(i);

			auto& memPair = saveTypesMem.at(i);
			memPair.first = StringToWideString(pair.first);
			memPair.second = StringToWideString(pair.second);

			saveTypes.push_back({ memPair.first.c_str(), memPair.second.c_str() });
		}

		IFileDialog* fd = NULL;
		IShellItem* resultItem = NULL;
		PWSTR selectedFilePath = NULL;
		string path;

		try
		{
			// Create the dialog
			AssertHResult(CoCreateInstance(dialogType, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fd)))

			// Set the flags for the dialog
			DWORD currentFlags;
			AssertHResult(fd->GetOptions(&currentFlags))
			AssertHResult(fd->SetOptions(currentFlags | flags))

			// Set the file types to display. This is a 1-based array.
			AssertHResult(fd->SetFileTypes(static_cast<UINT>(saveTypes.size()), saveTypes.data()))

			// Set the selected filter as the first
			AssertHResult(fd->SetFileTypeIndex(1))

			// Set the first extension as the default extension
			AssertHResult(fd->SetDefaultExtension(saveTypesMem.front().second.c_str()))

			// Show the dialog
			HRESULT showResult = fd->Show(owner);
			if (SUCCEEDED(showResult))
			{
				// Get the result when the user clicks the "Open" button
				AssertHResult(fd->GetResult(&resultItem))
				AssertHResult(resultItem->GetDisplayName(SIGDN_FILESYSPATH, &selectedFilePath))

				path = WideStringToString(selectedFilePath);
			}
			else if (showResult != ERROR_CANCELLED)
			{
				AssertHResult(showResult)
			}
		}
		catch (const std::exception& ex)
		{
			CocoError("Error showing open file dialog: {}", ex.what())
		}

		if (selectedFilePath)
			CoTaskMemFree(selectedFilePath);

		if (resultItem)
			resultItem->Release();

		if (fd)
			fd->Release();

		return path;
	}

	string Win32WindowExtensions::ShowOpenFileDialog(const std::vector<std::pair<const char*, const char*>>& filters)
	{
		Windowing::WindowService* windowing = Windowing::WindowService::Get();
		Assert(windowing)

		Ref<Window> mainWindow = windowing->GetMainWindow();
		Assert(mainWindow.IsValid())

		Win32Window* win32Window = static_cast<Win32Window*>(mainWindow.Get());

		return ShowDialog(CLSID_FileOpenDialog, FOS_FILEMUSTEXIST | FOS_NOCHANGEDIR | FOS_PATHMUSTEXIST, win32Window->_handle, filters);
	}

	string Win32WindowExtensions::ShowSaveFileDialog(const std::vector<std::pair<const char*, const char*>>& filters)
	{
		Windowing::WindowService* windowing = Windowing::WindowService::Get();
		Assert(windowing)

		Ref<Window> mainWindow = windowing->GetMainWindow();
		Assert(mainWindow.IsValid())

		Win32Window* win32Window = static_cast<Win32Window*>(mainWindow.Get());

		return ShowDialog(CLSID_FileSaveDialog, FOS_FILEMUSTEXIST | FOS_NOCHANGEDIR | FOS_PATHMUSTEXIST, win32Window->_handle, filters);
	}

#ifdef COCO_RENDERING_VULKAN
	SharedRef<Rendering::GraphicsPresenterSurface> Win32WindowExtensions::CreateVulkanSurfaceForWindow(const Win32Window& window) const
	{
		using namespace Coco::Rendering;
		using namespace Coco::Rendering::Vulkan;

		if (!RenderService::Get())
			throw std::exception("No RenderService has been created");

		Win32EnginePlatform& platform = static_cast<Win32EnginePlatform&>(Engine::Get()->GetPlatform());

		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = window.GetHandle();
		createInfo.hinstance = platform.GetHInstance();

		SharedRef<VulkanGraphicsPresenterSurface> surface = CreateSharedRef<VulkanGraphicsPresenterSurface>();

		VulkanGraphicsPlatform& graphicsPlatform = static_cast<VulkanGraphicsPlatform&>(RenderService::Get()->GetPlatform());
		AssertVkSuccess(vkCreateWin32SurfaceKHR(graphicsPlatform.GetVulkanInstance(), &createInfo, graphicsPlatform.GetAllocationCallbacks(), &(surface->Surface)))

		return surface;
	}
#endif
}