@echo off

pushd ..\
call vendor\premake\premake5.exe vs2022 --platform-win32 --services-all --renderRHIs-all --vulkan-sdk-path="C:\VulkanSDK\1.3.268.0" --high-dpi --tests
popd
pause