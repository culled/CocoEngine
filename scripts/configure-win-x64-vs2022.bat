@echo off

pushd ..\
call vendor\premake\premake5.exe vs2022 --vulkan-enabled --vulkan-sdk-path="C:\VulkanSDK\1.3.216.0"
popd
pause