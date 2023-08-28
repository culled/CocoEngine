#include "MaterialSerializer.h"

#include <Coco/Core/IO/File.h>
#include <Coco/Core/Resources/ResourceLibrary.h>
#include <Coco/Core/Logging/Logger.h>
#include "../Shader.h"
#include "../Texture.h"
#include "../RenderingService.h"
#include <sstream>
#include <Coco/Core/Types/UUID.h>

namespace Coco::Rendering
{
	string MaterialSerializer::Serialize(ResourceLibrary& library, const Ref<Resource>& resource)
	{
		if (const Material* material = dynamic_cast<const Material*>(resource.Get()))
		{
			std::stringstream stream;
			KeyValueWriter writer(stream);

			writer.WriteLine("version", "1");
			writer.WriteLine(s_materialNameVariable, material->GetName());

			writer.WriteLine(s_materialShaderVariable, material->GetShader() != nullptr ? material->GetShader()->GetFilePath() : "");
			
			writer.WriteLine(s_uniformsSection);
			writer.IncrementIndentLevel();

			const ShaderUniformData& uniforms = material->GetUniformData();

			for (const auto& kvp : uniforms.Floats)
			{
				writer.WriteLine(kvp.first);

				writer.IncrementIndentLevel();
				writer.WriteLine(s_uniformsTypeVariable, ToString(static_cast<int>(BufferDataFormat::Float)));
				writer.WriteLine(s_uniformsValueVariable, ToString(kvp.second));
				writer.DecrementIndentLevel();
			}

			for (const auto& kvp : uniforms.Vector2s)
			{
				writer.WriteLine(kvp.first);

				writer.IncrementIndentLevel();
				writer.WriteLine(s_uniformsTypeVariable, ToString(static_cast<int>(BufferDataFormat::Vector2)));
				writer.WriteLine(s_uniformsValueVariable, kvp.second.ToString());
				writer.DecrementIndentLevel();
			}

			for (const auto& kvp : uniforms.Vector3s)
			{
				writer.WriteLine(kvp.first);

				writer.IncrementIndentLevel();
				writer.WriteLine(s_uniformsTypeVariable, ToString(static_cast<int>(BufferDataFormat::Vector3)));
				writer.WriteLine(s_uniformsValueVariable, kvp.second.ToString());
				writer.DecrementIndentLevel();
			}

			for (const auto& kvp : uniforms.Vector4s)
			{
				writer.WriteLine(kvp.first);

				writer.IncrementIndentLevel();
				writer.WriteLine(s_uniformsTypeVariable, ToString(static_cast<int>(BufferDataFormat::Vector4)));
				writer.WriteLine(s_uniformsValueVariable, kvp.second.ToString());
				writer.DecrementIndentLevel();
			}

			for (const auto& kvp : uniforms.Ints)
			{
				writer.WriteLine(kvp.first);

				writer.IncrementIndentLevel();
				writer.WriteLine(s_uniformsTypeVariable, ToString(static_cast<int>(BufferDataFormat::Int)));
				writer.WriteLine(s_uniformsValueVariable, ToString(kvp.second));
				writer.DecrementIndentLevel();
			}

			for (const auto& kvp : uniforms.Vector2Ints)
			{
				writer.WriteLine(kvp.first);

				writer.IncrementIndentLevel();
				writer.WriteLine(s_uniformsTypeVariable, ToString(static_cast<int>(BufferDataFormat::Vector2Int)));
				writer.WriteLine(s_uniformsValueVariable, kvp.second.ToString());
				writer.DecrementIndentLevel();
			}

			for (const auto& kvp : uniforms.Vector3Ints)
			{
				writer.WriteLine(kvp.first);

				writer.IncrementIndentLevel();
				writer.WriteLine(s_uniformsTypeVariable, ToString(static_cast<int>(BufferDataFormat::Vector3Int)));
				writer.WriteLine(s_uniformsValueVariable, kvp.second.ToString());
				writer.DecrementIndentLevel();
			}

			for (const auto& kvp : uniforms.Vector4Ints)
			{
				writer.WriteLine(kvp.first);

				writer.IncrementIndentLevel();
				writer.WriteLine(s_uniformsTypeVariable, ToString(static_cast<int>(BufferDataFormat::Vector4Int)));
				writer.WriteLine(s_uniformsValueVariable, kvp.second.ToString());
				writer.DecrementIndentLevel();
			}

			for (const auto& kvp : uniforms.Colors)
			{
				writer.WriteLine(kvp.first);

				writer.IncrementIndentLevel();
				writer.WriteLine(s_uniformsTypeVariable, ToString(static_cast<int>(BufferDataFormat::Color)));
				writer.WriteLine(s_uniformsValueVariable, kvp.second.ToString());
				writer.DecrementIndentLevel();
			}

			for (const auto& kvp : uniforms.Matrix4x4s)
			{
				writer.WriteLine(kvp.first);

				writer.IncrementIndentLevel();
				writer.WriteLine(s_uniformsTypeVariable, ToString(static_cast<int>(BufferDataFormat::Matrix4x4)));
				writer.WriteLine(s_uniformsValueVariable, kvp.second.ToString());
				writer.DecrementIndentLevel();
			}
			
			writer.DecrementIndentLevel();

			writer.Flush();
			return stream.str();
		}
		else
		{
			throw InvalidOperationException("Resource was not a material");
		}
	}

	ManagedRef<Resource> MaterialSerializer::Deserialize(ResourceLibrary& library, const string& data)
	{
		std::stringstream stream(data);
		KeyValueReader reader(stream);

		string id;
		string name;
		string shaderPath;
		ShaderUniformData uniforms{};

		while (reader.ReadLine())
		{
			if (reader.IsKey("version") && reader.GetValue() != "1")
				throw InvalidOperationException("Mismatching material versions");
			else if (reader.IsKey(s_materialIDVariable))
				id = reader.GetValue();
			else if (reader.IsKey(s_materialNameVariable))
				name = reader.GetValue();
			else if (reader.IsKey(s_materialShaderVariable))
				shaderPath = reader.GetValue();
			else if (reader.IsKey(s_uniformsSection))
				ReadUniformsSection(reader, library, uniforms);
		}

		ManagedRef<Material> material = CreateManagedRef<Material>(UUID(id), name);

		// Can't set properties without a shader
		if (shaderPath.empty())
		{
			LogWarning(library.GetLogger(), "Material did not have a shader file");
			return std::move(material);
		}

		Ref<Shader> shader = library.Load<Shader>(shaderPath);

		if (!shader.IsValid())
		{
			LogWarning(library.GetLogger(), FormattedString("Could not load shader at \"{}\"", shaderPath));
			return std::move(material);
		}

		material->SetShader(shader);
		material->SetUniformData(uniforms);

		return std::move(material);
	}

	void MaterialSerializer::ReadUniformsSection(
		KeyValueReader& reader,
		ResourceLibrary& library,
		ShaderUniformData& uniforms)
	{
		while (reader.ReadIfIsIndentLevel(1))
		{
			string name = reader.GetKey();
			int uniformType;
			string valueText;

			while (reader.ReadIfIsIndentLevel(2))
			{
				if (reader.IsKey(s_uniformsTypeVariable))
					uniformType = reader.GetVariableValueAsInt();
				else if (reader.IsKey(s_uniformsValueVariable))
					valueText = reader.GetValue();
			}

			switch (uniformType)
			{
			case static_cast<int>(BufferDataFormat::Float):
				uniforms.Floats[name] = atof(valueText.c_str());
				break;
			case static_cast<int>(BufferDataFormat::Vector2):
				uniforms.Vector2s[name] = Vector2::Parse(valueText);
				break;
			case static_cast<int>(BufferDataFormat::Vector3):
				uniforms.Vector3s[name] = Vector3::Parse(valueText);
				break;
			case static_cast<int>(BufferDataFormat::Vector4):
				uniforms.Vector4s[name] = Vector4::Parse(valueText);
				break;
			case static_cast<int>(BufferDataFormat::Int):
				uniforms.Ints[name] = atoi(valueText.c_str());
				break;
			case static_cast<int>(BufferDataFormat::Vector2Int):
				uniforms.Vector2Ints[name] = Vector2Int::Parse(valueText);
				break;
			case static_cast<int>(BufferDataFormat::Vector3Int):
				uniforms.Vector3Ints[name] = Vector3Int::Parse(valueText);
				break;
			case static_cast<int>(BufferDataFormat::Vector4Int):
				uniforms.Vector4Ints[name] = Vector4Int::Parse(valueText);
				break;
			case static_cast<int>(BufferDataFormat::Color):
				uniforms.Colors[name] = Color::Parse(valueText);
				break;
			case static_cast<int>(BufferDataFormat::Matrix4x4):
				uniforms.Matrix4x4s[name] = Matrix4x4::Parse(valueText);
				break;
			case s_textureType:
				{
					Ref<Texture> texture = library.Load<Texture>(valueText);
					uniforms.Textures[name] = texture->ID;
				}
				break;
			default:
				LogWarning(library.GetLogger(), FormattedString("Unsupported uniform type for {}", name));
				break;
			}
		}
	}
}
