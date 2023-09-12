@echo off

pushd ..\
call vendor\premake\premake5.exe vs2022 --platform-windows --services-all --renderRHIs-all --vulkan-sdk-path="C:\VulkanSDK\1.3.216.0" --high-dpi
popd
pause