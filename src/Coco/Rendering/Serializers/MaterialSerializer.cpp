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
			
			writer.WriteLine(s_propertiesSection);
			writer.IncrementIndentLevel();

			writer.WriteLine(s_vector4Section);
			writer.IncrementIndentLevel();
			for (const auto& vec4KVP : material->GetVector4Properties())
				writer.WriteLine(vec4KVP.first, vec4KVP.second.ToString());
			writer.DecrementIndentLevel();

			writer.WriteLine(s_colorSection);
			writer.IncrementIndentLevel();
			for (const auto& colorKVP : material->GetColorProperties())
				writer.WriteLine(colorKVP.first, colorKVP.second.ToString());
			writer.DecrementIndentLevel();

			writer.WriteLine(s_textureSection);
			writer.IncrementIndentLevel();
			for (const auto& textureKVP : material->GetTextureProperties())
				writer.WriteLine(textureKVP.first, textureKVP.second.IsValid() ? textureKVP.second->GetFilePath() : "");
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
		UnorderedMap<string, Vector4> vec4Properties;
		UnorderedMap<string, Color> colorProperties;
		UnorderedMap<string, string> textureProperties;

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
			else if (reader.IsKey(s_propertiesSection))
				ReadPropertiesSection(reader, vec4Properties, colorProperties, textureProperties);
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

		for (const auto& vec4Prop : vec4Properties)
			material->SetVector4(vec4Prop.first, vec4Prop.second);

		for (const auto& colorProp : colorProperties)
			material->SetColor(colorProp.first, colorProp.second);

		for (const auto& textureProp : textureProperties)
		{
			Ref<Texture> texture = library.Load<Texture>(textureProp.second);
			material->SetTexture(textureProp.first, texture);
		}

		return std::move(material);
	}

	void MaterialSerializer::ReadPropertiesSection(
		KeyValueReader& reader, 
		UnorderedMap<string, Vector4>& vec4Properties,
		UnorderedMap<string, Color>& colorProperties,
		UnorderedMap<string, string>& textureProperties)
	{
		while (reader.ReadIfIsIndentLevel(1))
		{
			if (reader.IsKey(s_vector4Section))
				ReadVector4Section(reader, vec4Properties);
			else if (reader.IsKey(s_colorSection))
				ReadColorSection(reader, colorProperties);
			else if (reader.IsKey(s_textureSection))
				ReadTextureSection(reader, textureProperties);
		}
	}

	void MaterialSerializer::ReadVector4Section(KeyValueReader& reader, UnorderedMap<string, Vector4>& vec4Properties)
	{
		while (reader.ReadIfIsIndentLevel(2))
		{
			vec4Properties[reader.GetKey()] = Vector4::Parse(reader.GetValue());
		}
	}

	void MaterialSerializer::ReadColorSection(KeyValueReader& reader, UnorderedMap<string, Color>& colorProperties)
	{
		while (reader.ReadIfIsIndentLevel(2))
		{
			colorProperties[reader.GetKey()] = Color::Parse(reader.GetValue());
		}
	}

	void MaterialSerializer::ReadTextureSection(KeyValueReader& reader, UnorderedMap<string, string>& textureProperties)
	{
		while (reader.ReadIfIsIndentLevel(2))
		{
			textureProperties[reader.GetKey()] = reader.GetValue();
		}
	}
}
