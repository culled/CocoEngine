#include "Renderpch.h"
#include "ShaderUniformLayout.h"
#include "GraphicsDevice.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	const ShaderUniformLayout ShaderUniformLayout::Empty = ShaderUniformLayout();

	ShaderUniformLayout::ShaderUniformLayout() :
		ShaderUniformLayout(std::span<const ShaderUniform>())
	{}
	
	ShaderUniformLayout::ShaderUniformLayout(const std::initializer_list<ShaderUniform>& uniforms) :
		ShaderUniformLayout(std::span<const ShaderUniform>(uniforms))
	{}

	//ShaderUniformLayout::ShaderUniformLayout(const std::initializer_list<ShaderUniform>& uniforms) :
	//	_totalSize(0),
	//	_uniforms(),
	//	_hash(0)
	//{
	//	uint32 order = 0;
	//	std::hash<string> strHasher;
	//
	//	for (const auto& u : uniforms)
	//	{
	//		auto result = _uniforms.try_emplace(u.Name, u);
	//
	//		if (!result.second)
	//		{
	//			CocoError("Duplicate uniform \"{}\" encountered", u.Name)
	//			continue;
	//		}
	//
	//		ShaderUniform& uniform = result.first->second;
	//		uniform.Order = order;
	//		++order;
	//
	//		_hash = Math::CombineHashes(
	//			_hash,
	//			strHasher(u.Name),
	//			static_cast<uint64>(u.BindingPoints),
	//			static_cast<uint64>(u.Type));
	//	}
	//}

	ShaderUniformLayout::ShaderUniformLayout(std::span<const ShaderUniform> uniforms) :
		_totalSize(0),
		_uniforms(),
		_hash(0)
	{
		uint32 order = 0;
		std::hash<string> strHasher;

		for (const auto& u : uniforms)
		{
			auto result = _uniforms.try_emplace(u.Name, u);

			if (!result.second)
			{
				CocoError("Duplicate uniform \"{}\" encountered", u.Name)
					continue;
			}

			ShaderUniform& uniform = result.first->second;
			uniform.Order = order;
			++order;

			_hash = Math::CombineHashes(
				_hash,
				strHasher(u.Name),
				static_cast<uint64>(u.BindingPoints),
				static_cast<uint64>(u.Type));
		}
	}

	ShaderStageFlags ShaderUniformLayout::GetUniformBindStages(bool dataUniforms, bool textureUniforms) const
	{
		return std::accumulate(_uniforms.begin(), _uniforms.end(),
			ShaderStageFlags::None,
			[dataUniforms, textureUniforms](ShaderStageFlags f, const auto& kvp)
			{
				bool data = IsDataShaderUniformType(kvp.second.Type);

				if ((data && dataUniforms) || (!data && textureUniforms))
				{
					f |= kvp.second.BindingPoints;
				}

				return f;
			}
		);
	}

	bool ShaderUniformLayout::NeedsDataCalculation() const
	{
		return _totalSize == 0 && HasUniformsOfType(true, false);
	}

	void ShaderUniformLayout::CalculateDataUniforms(const GraphicsDevice& device)
	{
		std::vector<ShaderUniform*> uniforms = GetDataUniforms();

		uint64 offset = 0;
		for (ShaderUniform* uniform : uniforms)
		{
			BufferDataType type = GetBufferDataType(uniform->Type);
			offset = GetOffsetForAlignment(offset, device.GetDataTypeAlignment(type));

			uniform->Offset = offset;
			uniform->DataSize = GetBufferDataTypeSize(type);

			offset += uniform->DataSize;
		}

		// Pad out the data size so they fill a block accessible by the minimum buffer alignment
		_totalSize = GetOffsetForAlignment(offset, device.GetFeatures().MinimumBufferAlignment);
	}

	bool ShaderUniformLayout::HasUniformsOfType(bool dataUniforms, bool textureUniforms) const
	{
		return std::any_of(_uniforms.begin(), _uniforms.end(), 
			[dataUniforms, textureUniforms](const auto& kvp)
			{
				bool isData = IsDataShaderUniformType(kvp.second.Type);
				return (dataUniforms && isData) || (textureUniforms && !isData);
			}
		);
	}

	std::vector<const ShaderUniform*> ShaderUniformLayout::GetUniforms(bool includeDataUniforms, bool includeTextureUniforms) const
	{
		std::vector<const ShaderUniform*> uniforms;
		uniforms.reserve(_uniforms.size());

		for (const auto& kvp : _uniforms)
		{
			if (IsDataShaderUniformType(kvp.second.Type))
			{
				if (includeDataUniforms)
					uniforms.emplace_back(&kvp.second);
			}
			else
			{
				if (includeTextureUniforms)
					uniforms.emplace_back(&kvp.second);
			}
		}

		std::sort(uniforms.begin(), uniforms.end(),
			[](const ShaderUniform* a, const ShaderUniform* b)
			{
				return a->Order < b->Order;
			});

		return uniforms;
	}

	std::vector<ShaderUniformValue> ShaderUniformLayout::GetDefaultValues() const
	{
		std::vector<ShaderUniformValue> defaultValues;
		
		std::transform(_uniforms.begin(), _uniforms.end(),
			std::back_inserter(defaultValues),
			[](const auto& kvp)
			{
				return kvp.second;
			});

		return defaultValues;
	}

	std::vector<uint8> ShaderUniformLayout::GetBufferData(std::span<const ShaderUniformValue> uniformValues) const
	{
		CocoAssert(!NeedsDataCalculation(), "Layout's buffer size has not been calculated")

		std::vector<uint8> data(_totalSize);
		std::vector<const ShaderUniform*> uniforms = GetUniforms(true, false);

		for (const ShaderUniform* uniform : uniforms)
		{
			auto it = std::find_if(uniformValues.begin(), uniformValues.end(),
				[uniform](const ShaderUniformValue& uniformValue)
				{
					return uniform->Name == uniformValue.Name;
				});

			if (it == uniformValues.end())
			{
				GetDataUniformBytes(*uniform, std::span<uint8>(data.begin() + uniform->Offset, uniform->DataSize));
			}
			else
			{
				GetDataUniformBytes(*it, std::span<uint8>(data.begin() + uniform->Offset, uniform->DataSize));
			}
		}

		return data;
	}

	std::unordered_map<string, SharedRef<Texture>> ShaderUniformLayout::GetTextures(std::span<const ShaderUniformValue> uniformValues) const
	{
		std::unordered_map<string, SharedRef<Texture>> textures;
		std::vector<const ShaderUniform*> uniforms = GetUniforms(false, true);

		for (const ShaderUniform* uniform : uniforms)
		{
			auto it = std::find_if(uniformValues.begin(), uniformValues.end(),
				[uniform](const ShaderUniformValue& uniformValue)
				{
					return uniform->Name == uniformValue.Name;
				});

			if (it == uniformValues.end())
			{
				textures[uniform->Name] = GetTextureUniformValue(*uniform);
			}
			else
			{
				textures[uniform->Name] = GetTextureUniformValue(*it);
			}
		}

		return textures;
	}

	std::vector<ShaderUniform*> ShaderUniformLayout::GetDataUniforms()
	{
		std::vector<ShaderUniform*> dataUniforms;
		dataUniforms.reserve(_uniforms.size());

		for(auto& kvp : _uniforms)
		{
			if (!IsDataShaderUniformType(kvp.second.Type))
				continue;

			dataUniforms.emplace_back(&kvp.second);
		}

		std::sort(dataUniforms.begin(), dataUniforms.end(),
			[](const ShaderUniform* a, const ShaderUniform* b)
			{
				return a->Order < b->Order;
			});

		return dataUniforms;
	}
}