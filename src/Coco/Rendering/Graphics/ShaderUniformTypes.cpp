#include "Renderpch.h"
#include "ShaderUniformTypes.h"

#include <Coco/Core/Engine.h>
#include "../RenderService.h"

namespace Coco::Rendering
{
	BufferDataType GetBufferDataType(ShaderUniformType dataUniform)
	{
		switch (dataUniform)
		{
		case ShaderUniformType::Float:
			return BufferDataType::Float;
		case ShaderUniformType::Float2:
			return BufferDataType::Float2;
		case ShaderUniformType::Float3:
			return BufferDataType::Float3;
		case ShaderUniformType::Float4:
		case ShaderUniformType::Color:
			return BufferDataType::Float4;
		case ShaderUniformType::Matrix4x4:
			return BufferDataType::Matrix4x4;
		case ShaderUniformType::Int:
			return BufferDataType::Int;
		case ShaderUniformType::Int2:
			return BufferDataType::Int2;
		case ShaderUniformType::Int3:
			return BufferDataType::Int3;
		case ShaderUniformType::Int4:
			return BufferDataType::Int4;
		case ShaderUniformType::Bool:
			return BufferDataType::Bool;
		default:
			break;
		}

		CocoAssert(false, "Uniform was not a data uniform")
		return BufferDataType::Float;
	}

	bool IsDataShaderUniformType(ShaderUniformType type)
	{
		return type != ShaderUniformType::Texture;
	}

	const char* GetShaderUniformTypeString(ShaderUniformType type)
	{
		switch (type)
		{
		case ShaderUniformType::Float:
			return "Float";
		case ShaderUniformType::Float2:
			return "Float2";
		case ShaderUniformType::Float3:
			return "Float3";
		case ShaderUniformType::Float4:
			return "Float4";
		case ShaderUniformType::Color:
			return "Color";
		case ShaderUniformType::Matrix4x4:
			return "Matrix4x4";
		case ShaderUniformType::Int:
			return "Int";
		case ShaderUniformType::Int2:
			return "Int2";
		case ShaderUniformType::Int3:
			return "Int3";
		case ShaderUniformType::Int4:
			return "Int4";
		case ShaderUniformType::Bool:
			return "Boolean";
		case ShaderUniformType::Texture:
			return "Texture";
		default:
			break;
		}

		Assert(false)
		return "Unknown";
	}

	ShaderUniform::ShaderUniform(const string& name, ShaderUniformType type, ShaderStageFlags bindPoints, std::any defaultValue) :
		Name(name),
		Type(type),
		BindingPoints(bindPoints),
		DefaultValue(defaultValue)
	{}

	ShaderUniform::ShaderUniform(const string& name, ShaderStageFlags bindPoints, float defaultValue) :
		ShaderUniform(name, ShaderUniformType::Float, bindPoints, defaultValue)
	{}

	ShaderUniform::ShaderUniform(const string& name, ShaderStageFlags bindPoints, const Vector2& defaultValue) :
		ShaderUniform(name, ShaderUniformType::Float2, bindPoints, defaultValue.AsFloatArray())
	{}

	ShaderUniform::ShaderUniform(const string& name, ShaderStageFlags bindPoints, const Vector3& defaultValue) :
		ShaderUniform(name, ShaderUniformType::Float3, bindPoints, defaultValue.AsFloatArray())
	{}

	ShaderUniform::ShaderUniform(const string& name, ShaderStageFlags bindPoints, const Vector4& defaultValue) :
		ShaderUniform(name, ShaderUniformType::Float4, bindPoints, defaultValue.AsFloatArray())
	{}

	ShaderUniform::ShaderUniform(const string& name, ShaderStageFlags bindPoints, const Color& defaultValue) :
		ShaderUniform(name, ShaderUniformType::Color, bindPoints, defaultValue.AsFloatArray(true))
	{}

	ShaderUniform::ShaderUniform(const string& name, ShaderStageFlags bindPoints, int defaultValue) :
		ShaderUniform(name, ShaderUniformType::Int, bindPoints, defaultValue)
	{}

	ShaderUniform::ShaderUniform(const string& name, ShaderStageFlags bindPoints, const Vector2Int& defaultValue) :
		ShaderUniform(name, ShaderUniformType::Int2, bindPoints, defaultValue.AsIntArray())
	{}

	ShaderUniform::ShaderUniform(const string& name, ShaderStageFlags bindPoints, const Vector3Int& defaultValue) :
		ShaderUniform(name, ShaderUniformType::Int3, bindPoints, defaultValue.AsIntArray())
	{}

	ShaderUniform::ShaderUniform(const string& name, ShaderStageFlags bindPoints, const Vector4Int& defaultValue) :
		ShaderUniform(name, ShaderUniformType::Int4, bindPoints, defaultValue.AsIntArray())
	{}

	ShaderUniform::ShaderUniform(const string& name, ShaderStageFlags bindPoints, const Matrix4x4& defaultValue) :
		ShaderUniform(name, ShaderUniformType::Matrix4x4, bindPoints, defaultValue.AsFloatArray())
	{}

	ShaderUniform::ShaderUniform(const string& name, ShaderStageFlags bindPoints, bool defaultValue) :
		ShaderUniform(name, ShaderUniformType::Bool, bindPoints, defaultValue)
	{}

	ShaderUniform::ShaderUniform(const string& name, ShaderStageFlags bindPoints, DefaultTextureType defaultValue) :
		ShaderUniform(name, ShaderUniformType::Texture, bindPoints, defaultValue)
	{}

	ShaderUniform::ShaderUniform(const string & name, ShaderStageFlags bindPoints, SharedRef<Texture> defaultValue) :
		ShaderUniform(name, ShaderUniformType::Texture, bindPoints, defaultValue)
	{}

	bool ShaderUniform::operator==(const ShaderUniform& other) const
	{
		return Name == other.Name && Type == other.Type && BindingPoints == other.BindingPoints;
	}

	ShaderUniformValue::ShaderUniformValue(const string& name, ShaderUniformType type, std::any value) :
		Name(name),
		Type(type),
		Value(value)
	{}

	ShaderUniformValue::ShaderUniformValue(const ShaderUniform& uniform) :
		ShaderUniformValue(uniform.Name, uniform.Type, uniform.DefaultValue)
	{}

	ShaderUniformValue::ShaderUniformValue(const string& name, float value) :
		ShaderUniformValue(name, ShaderUniformType::Float, value)
	{}

	ShaderUniformValue::ShaderUniformValue(const string& name, const Vector2& value) :
		ShaderUniformValue(name, ShaderUniformType::Float2, value.AsFloatArray())
	{}

	ShaderUniformValue::ShaderUniformValue(const string& name, const Vector3& value) :
		ShaderUniformValue(name, ShaderUniformType::Float3, value.AsFloatArray())
	{}

	ShaderUniformValue::ShaderUniformValue(const string& name, const Vector4& value) :
		ShaderUniformValue(name, ShaderUniformType::Float4, value.AsFloatArray())
	{}

	ShaderUniformValue::ShaderUniformValue(const string& name, const Color& value) :
		ShaderUniformValue(name, ShaderUniformType::Color, value.AsFloatArray(true))
	{}

	ShaderUniformValue::ShaderUniformValue(const string& name, int value) :
		ShaderUniformValue(name, ShaderUniformType::Int, value)
	{}

	ShaderUniformValue::ShaderUniformValue(const string& name, const Vector2Int& value) :
		ShaderUniformValue(name, ShaderUniformType::Int2, value.AsIntArray())
	{}

	ShaderUniformValue::ShaderUniformValue(const string& name, const Vector3Int& value) :
		ShaderUniformValue(name, ShaderUniformType::Int3, value.AsIntArray())
	{}

	ShaderUniformValue::ShaderUniformValue(const string& name, const Vector4Int& value) :
		ShaderUniformValue(name, ShaderUniformType::Int4, value.AsIntArray())
	{}

	ShaderUniformValue::ShaderUniformValue(const string& name, const Matrix4x4& value) :
		ShaderUniformValue(name, ShaderUniformType::Matrix4x4, value.AsFloatArray())
	{}

	ShaderUniformValue::ShaderUniformValue(const string& name, bool value) :
		ShaderUniformValue(name, ShaderUniformType::Bool, value)
	{}

	ShaderUniformValue::ShaderUniformValue(const string& name, SharedRef<Texture> value) :
		ShaderUniformValue(name, ShaderUniformType::Texture, value)
	{}

	template<>
	const bool& ShaderUniformValue::As() const
	{
		CocoAssert(Type == ShaderUniformType::Bool, "Value was not a bool")
		return *std::any_cast<bool>(&Value);
	}

	template<>
	const float& ShaderUniformValue::As() const
	{
		CocoAssert(Type == ShaderUniformType::Float, "Value was not a float")
		return *std::any_cast<float>(&Value);
	}

	template<>
	const Float2& ShaderUniformValue::As() const
	{
		CocoAssert(Type == ShaderUniformType::Float2, "Value was not a float2")
		return *std::any_cast<Float2>(&Value);
	}

	template<>
	const Float3& ShaderUniformValue::As() const
	{
		CocoAssert(Type == ShaderUniformType::Float3, "Value was not a float3")
		return *std::any_cast<Float3>(&Value);
	}

	template<>
	const Float4& ShaderUniformValue::As() const
	{
		CocoAssert(Type == ShaderUniformType::Float4 || Type == ShaderUniformType::Color, "Value was not a float4 or color")
		return *std::any_cast<Float4>(&Value);
	}

	template<>
	const int& ShaderUniformValue::As() const
	{
		CocoAssert(Type == ShaderUniformType::Int, "Value was not a int")
		return *std::any_cast<int>(&Value);
	}

	template<>
	const Int2& ShaderUniformValue::As() const
	{
		CocoAssert(Type == ShaderUniformType::Int2, "Value was not a int2")
		return *std::any_cast<Int2>(&Value);
	}

	template<>
	const Int3& ShaderUniformValue::As() const
	{
		CocoAssert(Type == ShaderUniformType::Int3, "Value was not a int3")
		return *std::any_cast<Int3>(&Value);
	}

	template<>
	const Int4& ShaderUniformValue::As() const
	{
		CocoAssert(Type == ShaderUniformType::Int4, "Value was not a int4")
		return *std::any_cast<Int4>(&Value);
	}

	template<>
	const FloatMatrix4x4& ShaderUniformValue::As() const
	{
		CocoAssert(Type == ShaderUniformType::Matrix4x4, "Value was not a Matrix4x4")
		return *std::any_cast<FloatMatrix4x4>(&Value);
	}

	template<>
	const SharedRef<Texture>& ShaderUniformValue::As() const
	{
		CocoAssert(Type == ShaderUniformType::Texture, "Value was not a Texture")
		return *std::any_cast<SharedRef<Texture>>(&Value);
	}

	std::vector<uint8> GetDataUniformBytes(const ShaderUniformValue& uniform)
	{
		CocoAssert(IsDataShaderUniformType(uniform.Type), "Uniform type was not a data uniform")

		// Resize the data container to fit the uniform data
		BufferDataType bufferType = GetBufferDataType(uniform.Type);
		const uint8 dataSize = GetBufferDataTypeSize(bufferType);
		std::vector<uint8> data(dataSize);

		GetDataUniformBytes(uniform, data);

		return data;
	}

	void GetDataUniformBytes(const ShaderUniformValue& uniform, std::span<uint8> data)
	{
		CocoAssert(IsDataShaderUniformType(uniform.Type), "Uniform type was not a data uniform")

		switch (uniform.Type)
		{
		case ShaderUniformType::Float:
		{
			if (const float* v = std::any_cast<const float>(&uniform.Value))
				Assert(memcpy_s(data.data(), data.size(), v, BufferFloatSize) == 0)
			return;
		}
		case ShaderUniformType::Float2:
		{
			if (const Float2* v = std::any_cast<const Float2>(&uniform.Value))
				Assert(memcpy_s(data.data(), data.size(), v->data(), BufferFloatSize * 2) == 0)
			return;
		}
		case ShaderUniformType::Float3:
		{
			if (const Float3* v = std::any_cast<const Float3>(&uniform.Value))
				Assert(memcpy_s(data.data(), data.size(), v->data(), BufferFloatSize * 3) == 0)
			return;
		}
		case ShaderUniformType::Float4:
		case ShaderUniformType::Color:
		{
			if (const Float4* v = std::any_cast<const Float4>(&uniform.Value))
				Assert(memcpy_s(data.data(), data.size(), v->data(), BufferFloatSize * 4) == 0)
			return;
		}
		case ShaderUniformType::Matrix4x4:
		{
			if (const FloatMatrix4x4* v = std::any_cast<const FloatMatrix4x4>(&uniform.Value))
				Assert(memcpy_s(data.data(), data.size(), v->data(), BufferFloatSize * Matrix4x4::CellCount) == 0)
			return;
		}
		case ShaderUniformType::Int:
		{
			if (const int* v = std::any_cast<const int>(&uniform.Value))
				Assert(memcpy_s(data.data(), data.size(), v, BufferIntSize) == 0)
			return;
		}
		case ShaderUniformType::Int2:
		{
			if (const Int2* v = std::any_cast<const Int2>(&uniform.Value))
				Assert(memcpy_s(data.data(), data.size(), v->data(), BufferIntSize * 2) == 0)
			return;
		}
		case ShaderUniformType::Int3:
		{
			if (const Int3* v = std::any_cast<const Int3>(&uniform.Value))
				Assert(memcpy_s(data.data(), data.size(), v->data(), BufferIntSize * 3) == 0)
			return;
		}
		case ShaderUniformType::Int4:
		{
			if (const Int4* v = std::any_cast<const Int4>(&uniform.Value))
				Assert(memcpy_s(data.data(), data.size(), v->data(), BufferIntSize * 4) == 0)
			return;
		}
		case ShaderUniformType::Bool:
		{
			if (const bool* v = std::any_cast<const bool>(&uniform.Value))
				Assert(memcpy_s(data.data(), data.size(), v, sizeof(bool)) == 0)
			return;
		}
		default:
			break;
		}

		CocoAssert(false, "Unsupported data type")
	}

	SharedRef<Texture> GetTextureUniformValue(const ShaderUniformValue& uniform)
	{
		CocoAssert(uniform.Type == ShaderUniformType::Texture, "Uniform was not a texture uniform")
		
		if (const SharedRef<Texture>* tex = std::any_cast<const SharedRef<Texture>>(&uniform.Value))
		{
			return *tex;
		}
		else if(const DefaultTextureType* texType = std::any_cast<const DefaultTextureType>(&uniform.Value))
		{
			RenderService* rendering = RenderService::Get();
			CocoAssert(rendering, "RenderService singleton was null")

			switch (*texType)
			{
			case DefaultTextureType::White:
				return rendering->GetDefaultDiffuseTexture();
			case DefaultTextureType::FlatNormal:
				return rendering->GetDefaultNormalTexture();
			default:
				return rendering->GetDefaultCheckerTexture();
			}
		}
		else
		{
			CocoAssert(false, FormatString("Texture uniform \"{}\" did not contain a SharedRef<Texture> or DefaultTextureType", uniform.Name))
			return nullptr;
		}
	}
}