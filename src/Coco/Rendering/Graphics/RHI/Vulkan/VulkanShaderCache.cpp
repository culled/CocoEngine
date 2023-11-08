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

	//Shader VulkanShaderCache::LoadShader(const std::unordered_map<ShaderStageType, string>& stageFiles)
	//{
	//	Shader shader;
	//
	//	for (const auto& stageFile : stageFiles)
	//	{
	//		if (variant.Name.empty())
	//		{
	//			FilePath stageFilePath(stageFile.second);
	//			variant.Name = stageFilePath.GetFileName(false);
	//		}
	//
	//		//ShaderStage stage("main", stageFile.first, stageFile.second);
	//		ShaderStage& stage = variant.Stages.emplace_back("main", stageFile.first, stageFile.second);
	//		std::vector<uint32> byteCode = CompileOrGetShaderStageBinary(stage);
	//		Reflect(variant, stage, byteCode);
	//	}
	//
	//	return shader;
	//}

	VulkanShader& VulkanShaderCache::GetOrCreateShader(const SharedRef<Shader>& shader)
	{
		GraphicsDeviceResourceID key = VulkanShader::MakeKey(shader);

		auto it = _shaders.find(key);

		if (it == _shaders.end())
		{
			it = _shaders.try_emplace(key, shader).first;
		}

		VulkanShader& resource = it->second;

		if (resource.NeedsUpdate())
			resource.Update();

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
			stage.CompiledFilePath.IsEmpty() ? 
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

			const bool optimize = false;
			if (optimize)
				options.SetOptimizationLevel(shaderc_optimization_level_performance);

			string filePathStr = stage.SourceFilePath.ToString();
			shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(shaderSource, ShaderStageToShaderC(stage.Type), filePathStr.c_str(), options);

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

	/*void VulkanShaderCache::Reflect(ShaderVariant& variant, const ShaderStage& stage, const std::vector<uint32>& byteCode)
	{
		spirv_cross::Compiler compiler(byteCode.data(), byteCode.size());
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		CocoInfo("VulkanShaderCache::Reflect - {}", stage.SourceFilePath);
		CocoInfo("\t{} uniform buffers", resources.uniform_buffers.size());

		for (const auto& buffer : resources.uniform_buffers)
		{
			const auto& type = compiler.get_type(buffer.base_type_id);
			uint64 size = compiler.get_declared_struct_size(type);
			uint32 set = compiler.get_decoration(buffer.id, spv::DecorationDescriptorSet);
			uint32 binding = compiler.get_decoration(buffer.id, spv::DecorationBinding);
			uint64 memberCount = type.member_types.size();

			if (binding == 0)
			{
				switch (set)
				{
				case 1:
					ReflectUniforms(variant.GlobalUniforms, stage.Type, compiler, buffer);
					break;
				case 2:
					ReflectUniforms(variant.InstanceUniforms, stage.Type, compiler, buffer);
					break;
				case 3:
					ReflectUniforms(variant.DrawUniforms, stage.Type, compiler, buffer);
					break;
				default:
					CocoInfo("Skipping unsupported uniform set {}", set)
					break;
				}
			}
			else if(binding == 1)
			{
				if (set > 1)
				{
					CocoError("Uniform buffer blocks are only available for sets 0 and 1")
					continue;
				}
				else if (set == 1)
				{
					ReflectUniformBlock(variant.GlobalUniforms, stage.Type, compiler, buffer);
				}
				else
				{
					CocoInfo("Skipping unsupported uniform buffer set {}", set)
				}
			}
			else
			{
				CocoInfo("Skipping unsupported uniform binding {}", binding)
			}

			CocoInfo("\t\t{}:", buffer.name)
			CocoInfo("\t\t\tSize = {}", size)
			CocoInfo("\t\t\tSet = {}", set)
			CocoInfo("\t\t\tBinding = {}", binding)
			CocoInfo("\t\t\tMembers = {}", memberCount)
			for (uint32 i = 0; i < memberCount; i++)
			{
				CocoInfo("\t\t\t\t{}:", compiler.get_member_name(buffer.base_type_id, i))
				const auto& memberType = compiler.get_type(type.member_types[i]);

				if (memberType.basetype != spirv_cross::SPIRType::BaseType::Struct)
				{
					CocoInfo("\t\t\t\t\tType = {}", GetBufferDataTypeString(SPIRTypeToBufferDataType(memberType)))
				}
				else
				{
					CocoInfo("\t\t\t\t\tType = Struct")
					CocoInfo("\t\t\t\t\tSize = {}", compiler.get_declared_struct_size(memberType))
				}
			}
		}

		CocoInfo("\t{} images", resources.sampled_images.size());
		for (const auto& image : resources.sampled_images)
		{
			uint32 set = compiler.get_decoration(image.id, spv::DecorationDescriptorSet);
			uint32 binding = compiler.get_decoration(image.id, spv::DecorationBinding);

			CocoInfo("\t\t{}:", image.name)
			CocoInfo("\t\t\tSet = {}", set)
			CocoInfo("\t\t\tBinding = {}", binding)

			ReflectTexture(variant, stage.Type, compiler, image);
		}
	}

	void VulkanShaderCache::ReflectUniforms(
		ShaderUniformLayout& layout,
		ShaderStageType stage, 
		const spirv_cross::Compiler& compiler, 
		const spirv_cross::Resource& bufferResource)
	{
		const auto& type = compiler.get_type(bufferResource.base_type_id);
		uint64 memberCount = type.member_types.size();
		ShaderStageFlags stageFlags = ToShaderStageFlags(stage);

		// TODO: reflect uniforms
		for (uint32 i = 0; i < memberCount; i++)
		{
			const auto& memberType = compiler.get_type(type.member_types[i]);

			if (memberType.basetype == spirv_cross::SPIRType::BaseType::Struct)
			{
				throw std::exception("Structs are not supported in uniform data blocks");
			}
			else
			{
				string name = compiler.get_member_name(bufferResource.base_type_id, i);
				BufferDataType bufferDataType = SPIRTypeToBufferDataType(memberType);

				auto it = std::find_if(layout.Uniforms.begin(), layout.Uniforms.end(), [name](const auto& other)
					{
						return other.Name == name;
					}
				);

				if (it != layout.Uniforms.end())
				{
					if (it->Type != bufferDataType)
					{
						string err = FormatString("A uniform named \"{}\" already exists", name);
						throw std::exception(err.c_str());
					}

					it->BindingPoints |= stageFlags;
					continue;
				}

				layout.DataUniforms.emplace_back(name, stageFlags, bufferDataType);
			}
		}
	}
	void VulkanShaderCache::ReflectUniformBlock(
		GlobalShaderUniformLayout& layout, 
		ShaderStageType stage, 
		const spirv_cross::Compiler& compiler, 
		const spirv_cross::Resource& bufferResource)
	{
		const auto& type = compiler.get_type(bufferResource.base_type_id);
		uint64 memberCount = type.member_types.size();
		ShaderStageFlags stageFlags = ToShaderStageFlags(stage);

		for (uint32 i = 0; i < memberCount; i++)
		{
			const auto& memberType = compiler.get_type(type.member_types[i]);
			string name = compiler.get_member_name(bufferResource.base_type_id, i);

			if (memberType.array.size() > 1)
			{
				throw std::exception("Multidimensional arrays are not supported");
			}

			auto it = std::find_if(layout.BufferUniforms.begin(), layout.BufferUniforms.end(), [name](const auto& other)
				{
					return other.Name == name;
				}
			);

			uint64 size = compiler.get_declared_struct_size(memberType);

			if (memberType.array.size() > 0)
			{
				size *= memberType.array.front();
			}

			if (it != layout.BufferUniforms.end())
			{
				if (it->Size == size)
				{
					it->BindingPoints |= stageFlags;
					continue;
				}
				else
				{
					string err = FormatString("A buffer uniform named \"{}\" already exists", name);
					throw std::exception(err.c_str());
				}

				layout.BufferUniforms.emplace_back(name, stageFlags, size);
			}
		}
	}

	void VulkanShaderCache::ReflectTexture(
		ShaderVariant& variant, 
		ShaderStageType stage, 
		const spirv_cross::Compiler& compiler, 
		const spirv_cross::Resource& imageResource)
	{
		// TODO: reflect uniforms
		uint32 set = compiler.get_decoration(imageResource.id, spv::DecorationDescriptorSet);

		if (set <= 0 || set > 3)
			return;

		ShaderUniformLayout& layout = set == 1 ? variant.GlobalUniforms : (
			set == 2 ? variant.InstanceUniforms : variant.DrawUniforms
			);

		uint32 binding = compiler.get_decoration(imageResource.id, spv::DecorationBinding);
		ShaderStageFlags stageFlags = ToShaderStageFlags(stage);

		auto it = std::find_if(layout.TextureUniforms.begin(), layout.TextureUniforms.end(), [imageResource](const auto& o)
			{
				return o.Name == imageResource.name;
			}
		);

		if (it != layout.TextureUniforms.end())
		{
			// TODO: check if the binding spot is the same	
			it->BindingPoints |= stageFlags;
			return;

			//string err = FormatString("A texture for set {} named \"{}\" already exists", set, imageResource.name);
			//throw std::exception(err.c_str());
		}

		layout.TextureUniforms.emplace_back(imageResource.name, stageFlags, ShaderTextureUniform::DefaultTextureType::White);
	}*/
}