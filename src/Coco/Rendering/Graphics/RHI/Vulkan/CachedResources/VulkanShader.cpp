#include "Renderpch.h"
#include "VulkanShader.h"
#include "../VulkanGraphicsDevice.h"
#include "../VulkanUtils.h"
#include "../../../../RenderService.h"
#include <Coco/Core/Engine.h>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>

namespace Coco::Rendering::Vulkan
{
	VulkanShaderStage::VulkanShaderStage(const ShaderStage& stage) :
		ShaderStage(stage),
		ShaderModule(nullptr)
	{}

	const string VulkanShader::CacheDirectory = "cache/shaders/vulkan";

	VulkanShader::VulkanShader(uint64 id, VulkanGraphicsDevice& device, const SharedRef<Shader>& shader) :
		CachedVulkanResource(id),
		_device(device),
		_shader(shader),
		_stages(),
		_pushConstantRanges(),
		_version(0)
	{}

	VulkanShader::~VulkanShader()
	{
		DestroyShaderModules();
	}

	void VulkanShader::Use()
	{
		CocoAssert(!_shader.expired(), "Base shader expired")
		SharedRef<Shader> shader = _shader.lock();

		if (_version != shader->GetVersion() || _stages.size() == 0)
		{
			DestroyShaderModules();
			CreateShaderModules();
			CalculatePushConstantRanges();

			_version = shader->GetVersion();
		}

		CachedVulkanResource::Use();
	}

	bool VulkanShader::IsStale(double staleThreshold) const
	{
		if (_shader.expired())
			return true;

		return CachedVulkanResource::IsStale(staleThreshold);
	}

	uint64 VulkanShader::MakeKey(const Shader& shader)
	{
		return shader.GetID();
	}

	bool VulkanShader::HasScope(UniformScope scope) const
	{
		if (_shader.expired())
			return false;

		SharedRef<Shader> shader = _shader.lock();

		switch (scope)
		{
		case UniformScope::ShaderGlobal:
			return shader->GetGlobalUniformLayout() != ShaderUniformLayout::Empty;
		case UniformScope::Instance:
			return shader->GetInstanceUniformLayout() != ShaderUniformLayout::Empty;
		case UniformScope::Draw:
			return shader->GetDrawUniformLayout() != ShaderUniformLayout::Empty;
		default:
			return false;
		}
	}

	const ShaderUniformLayout& VulkanShader::GetUniformLayout(UniformScope scope) const
	{
		if (_shader.expired())
			return ShaderUniformLayout::Empty;

		SharedRef<Shader> shader = _shader.lock();

		switch (scope)
		{
		case UniformScope::ShaderGlobal:
			return shader->GetGlobalUniformLayout();
		case UniformScope::Instance:
			return shader->GetInstanceUniformLayout();
		case UniformScope::Draw:
			return shader->GetDrawUniformLayout();
		default:
			return ShaderUniformLayout::Empty;
		}
	}

	shaderc_shader_kind VulkanShader::ShaderStageToShaderC(ShaderStageType stage)
	{
		switch (stage)
		{
		case ShaderStageType::Vertex:
			return shaderc_vertex_shader;
		case ShaderStageType::Tesselation:
			return shaderc_tess_control_shader;
		case ShaderStageType::Geometry:
			return shaderc_geometry_shader;
		case ShaderStageType::Fragment:
			return shaderc_fragment_shader;
		case ShaderStageType::Compute:
			return shaderc_compute_shader;
		default:
			return shaderc_vertex_shader;
		}
	}

	std::vector<uint32> VulkanShader::CompileOrGetShaderStageBinary(ShaderStage& stage)
	{
		FilePath sourceFilePath(stage.SourceFilePath);
		FilePath cachedFilePath(
			stage.CompiledFilePath.IsEmpty() ?
			FilePath::CombineToPath(CacheDirectory, sourceFilePath.GetFileName(false) + ".spv") :
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

			RenderService* rendering = RenderService::Get();
			CocoAssert(rendering, "RenderService singleton was null")
			Version apiVersion = rendering->GetPlatform().GetAPIVersion();

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

			const bool optimize = false;
			if (optimize)
				options.SetOptimizationLevel(shaderc_optimization_level_performance);

			string filePathStr = stage.SourceFilePath.ToString();
			shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(shaderSource, ShaderStageToShaderC(stage.Type), filePathStr.c_str(), options);

			if (result.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				throw Exception(result.GetErrorMessage());
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

	void VulkanShader::CreateShaderModules()
	{
		SharedRef<Shader> shader = _shader.lock();

		for (const ShaderStage& stage : shader->GetStages())
		{
			VulkanShaderStage& vulkanStage = _stages.emplace_back(stage);

			VkShaderModuleCreateInfo createInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
			std::vector<uint32> byteCode = CompileOrGetShaderStageBinary(vulkanStage);

			createInfo.codeSize = byteCode.size() * sizeof(uint32);
			createInfo.pCode = byteCode.data();

			AssertVkSuccess(vkCreateShaderModule(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &vulkanStage.ShaderModule))
		}

		CocoTrace("Created VulkanShader {} data for shader \"{}\"", ID, shader->GetName())
	}

	void VulkanShader::DestroyShaderModules()
	{
		if (_stages.size() == 0)
			return;

		_device.WaitForIdle();
		for (const VulkanShaderStage& stage : _stages)
		{
			vkDestroyShaderModule(_device.GetDevice(), stage.ShaderModule, _device.GetAllocationCallbacks());
		}

		_stages.clear();

		CocoTrace("Destroyed VulkanShader {} data", ID)
	}

	void VulkanShader::CalculatePushConstantRanges()
	{
		SharedRef<Shader> shader = _shader.lock();

		_pushConstantRanges.clear();

		const ShaderUniformLayout& drawLayout = shader->GetDrawUniformLayout();
		const uint64 dataSize = drawLayout.GetTotalDataSize();

		if (dataSize == 0)
			return;

		const VulkanGraphicsDeviceFeatures& features = static_cast<const VulkanGraphicsDeviceFeatures&>(_device.GetFeatures());

		if (dataSize > features.MaxPushConstantSize)
		{
			CocoError("Cannot have a push constant buffer greater than the maximum supported {} bytes. Requested: {} bytes", features.MaxPushConstantSize, dataSize)
			return;
		}

		std::vector<const ShaderUniform*> layoutUniforms = drawLayout.GetUniforms(true, false);
		uint32 rangeStartOffset = 0;
		ShaderStageFlags currentStageFlags = layoutUniforms.front()->BindingPoints;

		for (const ShaderUniform* uniform : layoutUniforms)
		{
			if (uniform->BindingPoints != currentStageFlags)
			{
				VkPushConstantRange& range = _pushConstantRanges.emplace_back();
				range.offset = rangeStartOffset;
				range.size = static_cast<uint32>(uniform->Offset) - rangeStartOffset;
				range.stageFlags = ToVkShaderStageFlags(currentStageFlags);

				rangeStartOffset = static_cast<uint32>(uniform->Offset);
				currentStageFlags = uniform->BindingPoints;
			}
		}

		VkPushConstantRange& range = _pushConstantRanges.emplace_back();
		range.offset = rangeStartOffset;
		range.size = static_cast<uint32>(dataSize) - rangeStartOffset;
		range.stageFlags = ToVkShaderStageFlags(currentStageFlags);
	}
}