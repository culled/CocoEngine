#include "Renderpch.h"
#include "VulkanShaderCache.h"

#include "VulkanGraphicsDeviceCache.h"
#include "../../../RenderService.h"
#include "../../GraphicsUtils.h"
#include <Coco/Core/Engine.h>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>

namespace Coco::Rendering::Vulkan
{
	const string VulkanShaderCache::_sCacheDirectory = "cache/shaders/vulkan";

	VulkanShaderCache::VulkanShaderCache() :
		_shaders(),
		_lastPurgeTime(0)
	{}

	VulkanShaderCache::~VulkanShaderCache()
	{
		_shaders.clear();
	}

	ShaderVariant VulkanShaderCache::LoadVariant(const std::unordered_map<ShaderStageType, string>& stageFiles)
	{
		for (const auto& stageFile : stageFiles)
		{
			ShaderStage stage("main", stageFile.first, stageFile.second);
			std::vector<uint32> byteCode = CompileOrGetShaderStageBinary(stage);
			// TODO: reflection
			//Reflect(stage, byteCode);
		}

		return ShaderVariant("", {}, GraphicsPipelineState(), {}, {}, {}, {}, {});
	}

	VulkanShaderVariant& VulkanShaderCache::GetOrCreateShader(const ShaderVariantData& variantData)
	{
		GraphicsDeviceResourceID key = VulkanShaderVariant::MakeKey(variantData);

		auto it = _shaders.find(key);

		if (it == _shaders.end())
		{
			it = _shaders.try_emplace(key, variantData).first;
		}

		VulkanShaderVariant& resource = it->second;

		if (resource.NeedsUpdate(variantData))
			resource.Update(variantData);

		resource.Use();

		return resource;
	}

	void VulkanShaderCache::ResetForNextFrame()
	{
		if (MainLoop::cGet()->GetCurrentTick().UnscaledTime - _lastPurgeTime > VulkanGraphicsDeviceCache::sPurgePeriod)
			PurgeStaleResources();
	}

	void VulkanShaderCache::PurgeStaleResources()
	{
		uint64 shadersPurged = 0;

		{
			auto it = _shaders.begin();

			while (it != _shaders.end())
			{
				if (it->second.IsStale(VulkanGraphicsDeviceCache::sPurgeThreshold))
				{
					it = _shaders.erase(it);
					shadersPurged++;
				}
				else
				{
					++it;
				}
			}
		}

		if (shadersPurged > 0)
		{
			CocoTrace("Purged {} VulkanRenderPassShaders", shadersPurged)
		}

		_lastPurgeTime = MainLoop::cGet()->GetCurrentTick().UnscaledTime;
	}

	std::vector<uint32> VulkanShaderCache::CompileOrGetShaderStageBinary(ShaderStage& stage)
	{
		FilePath sourceFilePath(stage.SourceFilePath);
		FilePath cachedFilePath(
			stage.CompiledFilePath.empty() ? 
			FilePath::CombineToPath(_sCacheDirectory, sourceFilePath.GetFileName(false) + ".spv") :
			stage.CompiledFilePath
		);

		std::vector<uint32> byteCode;

		EngineFileSystem& fs = Engine::Get()->GetFileSystem();

		if (fs.FileExists(cachedFilePath.ToString()))
		{
			File cacheFile = fs.OpenFile(cachedFilePath.ToString(), FileOpenFlags::Read);
			std::vector<uint8> temp = cacheFile.ReadToEnd();
			byteCode.resize(temp.size() / sizeof(uint32));
			Assert(memcpy_s(byteCode.data(), byteCode.size() * sizeof(uint32), temp.data(), temp.size()) == 0)
			cacheFile.Close();
		}
		else
		{
			File shaderFile = fs.OpenFile(stage.SourceFilePath, FileOpenFlags::Read | FileOpenFlags::Text);
			string shaderSource = shaderFile.ReadTextToEnd();
			shaderFile.Close();

			CocoInfo("Compiling shader variant: {}", sourceFilePath.ToString())

			Version apiVersion = RenderService::Get()->GetPlatform().GetAPIVersion();

			uint32 envVersion;
			switch (apiVersion.Minor)
			{
			case 1:
				envVersion = shaderc_env_version_vulkan_1_1;
				break;
			case 2:
				envVersion = shaderc_env_version_vulkan_1_2;
				break;
			case 3:
				envVersion = shaderc_env_version_vulkan_1_3;
				break;
			default:
				envVersion = shaderc_env_version_vulkan_1_0;
				break;
			}

			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
			options.SetTargetEnvironment(shaderc_target_env_vulkan, envVersion);

			const bool optimize = true;
			if (optimize)
				options.SetOptimizationLevel(shaderc_optimization_level_performance);

			shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(shaderSource, ShaderStageToShaderC(stage.Type), stage.SourceFilePath.c_str(), options);

			if (result.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				string err = FormatString("Error compiling shader: {}", result.GetErrorMessage());
				throw std::exception(err.c_str());
			}

			byteCode = std::vector<uint32>(result.begin(), result.end());
			uint64 byteCodeSize = byteCode.size() * sizeof(uint32);
			std::vector<uint8> temp(byteCodeSize);

			Assert(memcpy_s(temp.data(), temp.size(), byteCode.data(), byteCodeSize) == 0)

			File cacheFile = fs.CreateFile(cachedFilePath.ToString(), FileOpenFlags::Write);
			cacheFile.Write(temp);
			cacheFile.Flush();
			cacheFile.Close();
		}

		stage.CompiledFilePath = cachedFilePath.ToString();

		return byteCode;
	}

	void VulkanShaderCache::Reflect(ShaderStage& stage, const std::vector<uint32>& byteCode)
	{
		spirv_cross::Compiler compiler(byteCode.data(), byteCode.size());
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		CocoInfo("VulkanShaderCache::Reflect - {}", stage.SourceFilePath);
		CocoInfo("\t{} uniform buffers", resources.uniform_buffers.size());

		for (const auto& buffer : resources.uniform_buffers)
		{
			const auto& type = compiler.get_type(buffer.base_type_id);
			uint64 size = compiler.get_declared_struct_size(type);
			uint32 binding = compiler.get_decoration(buffer.id, spv::DecorationBinding);
			uint64 memberCount = type.member_types.size();

			CocoInfo("\t\t{}:", buffer.name);
			CocoInfo("\t\t\tSize = {}", size);
			CocoInfo("\t\t\tBinding = {}", binding);
			CocoInfo("\t\t\tMembers = {}", memberCount);
			for (uint32 i = 0; i < memberCount; i++)
			{
				CocoInfo("\t\t\t\t{}:", compiler.get_member_name(buffer.base_type_id, i))
				const auto& memberType = compiler.get_type(type.member_types[i]);

				CocoInfo("\t\t\t\t\tType = {}", GetBufferDataTypeString(SPIRTypeToBufferDataType(memberType)))
			}
		}

		CocoInfo("\t{} images", resources.sampled_images.size());
		for (const auto& image : resources.sampled_images)
		{
			CocoInfo("\t\t{}", image.name);
		}
	}
}