#include "MaterialLoader.h"

#include <Coco/Core/IO/File.h>
#include <Coco/Core/Resources/ResourceLibrary.h>
#include <Coco/Core/Logging/Logger.h>
#include "../Material.h"
#include "../Shader.h"
#include "../Texture.h"

namespace Coco::Rendering
{
	MaterialLoader::MaterialLoader(ResourceLibrary* library, const string& basePath) : KeyValueResourceLoader(library, basePath)
	{}

	Ref<Resource> MaterialLoader::LoadImpl(const string& path)
	{
		File file = File::Open(path, FileModes::Read);
		KeyValueReader reader(file);

		string name;
		string shaderPath;
		Map<string, Vector4> vec4Properties;
		Map<string, string> textureProperties;

		while (reader.ReadLine())
		{
			if (reader.IsVariable("version") && reader.GetVariableValue() != "1")
				throw InvalidOperationException("Mismatching material versions");
			else if (reader.IsVariable(s_materialNameVariable))
				name = reader.GetVariableValue();
			else if (reader.IsVariable(s_materialShaderVariable))
				shaderPath = reader.GetVariableValue();
			else if (reader.IsVariable(s_propertiesSection))
				ReadPropertiesSection(reader, vec4Properties, textureProperties);
		}

		// Can't set properties without a shader
		if (shaderPath.empty())
		{
			LogWarning(Library->GetLogger(), FormattedString("\"{}\" did not have a shader file", path));
			return CreateRef<Material>(nullptr, name);
		}

		Ref<Shader> shader = static_pointer_cast<Shader>(Library->GetOrLoadResource(ResourceType::Shader, shaderPath));
		if (shader == nullptr)
		{
			LogWarning(Library->GetLogger(), FormattedString("Could not load shader at \"{}\" for \"{}\"", shaderPath, path));
			return CreateRef<Material>(nullptr, name);
		}

		Ref<Material> material = CreateRef<Material>(shader, name);

		for (const auto& vec4Prop : vec4Properties)
			material->SetVector4(vec4Prop.first, vec4Prop.second);

		for (const auto& textureProp : textureProperties)
		{
			Ref<Texture> texture = static_pointer_cast<Texture>(Library->GetOrLoadResource(ResourceType::Texture, textureProp.second));
			material->SetTexture(textureProp.first, texture);
		}

		return material;
	}

	void MaterialLoader::SaveImpl(const Ref<Resource>&resource, const string & path)
	{
		if (Material* material = dynamic_cast<Material*>(resource.get()))
		{
			File file = File::Open(path, FileModes::Write);
			KeyValueWriter writer(file);

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

			writer.WriteLine(s_textureSection);
			writer.IncrementIndentLevel();
			for (const auto& textureKVP : material->GetTextureProperties())
				writer.WriteLine(textureKVP.first, textureKVP.second != nullptr ? textureKVP.second->GetFilePath() : "");
			writer.DecrementIndentLevel();

			file.Close();
		}
		else
		{
			throw InvalidOperationException("Resource was not a material");
		}
	}

	void MaterialLoader::ReadPropertiesSection(KeyValueReader& reader, Map<string, Vector4>& vec4Properties, Map<string, string>& textureProperties)
	{
		while (reader.ReadIfIsIndentLevel(1))
		{
			if (reader.IsVariable(s_vector4Section))
				ReadVector4Section(reader, vec4Properties);
			else if (reader.IsVariable(s_textureSection))
				ReadTextureSection(reader, textureProperties);
		}
	}

	void MaterialLoader::ReadVector4Section(KeyValueReader& reader, Map<string, Vector4>& vec4Properties)
	{
		while (reader.ReadIfIsIndentLevel(2))
		{
			vec4Properties[reader.GetVariable()] = Vector4::Parse(reader.GetVariableValue());
		}
	}

	void MaterialLoader::ReadTextureSection(KeyValueReader& reader, Map<string, string>& textureProperties)
	{
		while (reader.ReadIfIsIndentLevel(2))
		{
			textureProperties[reader.GetVariable()] = reader.GetVariableValue();
		}
	}
}
