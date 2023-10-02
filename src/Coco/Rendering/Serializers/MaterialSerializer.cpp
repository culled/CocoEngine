#include "Renderpch.h"
#include "MaterialSerializer.h"

#include "../Material.h"
#include "../MaterialInstance.h"
#include "Types/ShaderUniformDataSerializer.h"

#include <yaml-cpp/yaml.h>

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	bool MaterialSerializer::SupportsFileExtension(const string& extension) const
	{
		return extension == ".cmaterial" || extension == ".cmaterialinst";
	}

	bool MaterialSerializer::SupportsResourceType(const std::type_index& type) const
	{
		return type == typeid(Material) || type == typeid(MaterialInstance);
	}

	const std::type_index MaterialSerializer::GetResourceTypeForExtension(const string& extension) const
	{
		return extension == ".cmaterial" ? typeid(Material) : typeid(MaterialInstance);
	}

	string MaterialSerializer::Serialize(Ref<Resource> resource)
	{
		if (const Material* material = dynamic_cast<const Material*>(resource.Get()))
			return SerializeMaterial(*material);
		else if (const MaterialInstance* materialInst = dynamic_cast<const MaterialInstance*>(resource.Get()))
			return SerializeMaterialInstance(*materialInst);

		throw std::exception("Unsupported resource type");
	}

	ManagedRef<Resource> MaterialSerializer::Deserialize(const std::type_index& resourceType, const ResourceID& resourceID, const string& data)
	{
		if (resourceType == typeid(Material))
			return DeserializeMaterial(resourceID, data);
		else if (resourceType == typeid(MaterialInstance))
			return DeserializeMaterialInstance(resourceID, data);

		throw std::exception("Unsupported resource type");
	}

	string MaterialSerializer::SerializeMaterial(const Material& material)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "name" << YAML::Value << material.GetName();

		Ref<Shader> shader = material.GetShader();
		Assert(shader.IsValid())

		out << YAML::Key << "shader path" << YAML::Value << shader->GetContentPath();

		out << YAML::Key << "uniforms" << YAML::Value << YAML::BeginMap;

		ShaderUniformDataSerializer::Serialize(out, material._uniformData, material._textures);

		out << YAML::EndMap;

		out << YAML::EndMap << YAML::Comment("Fix");

		return string(out.c_str());
	}

	ManagedRef<Resource> MaterialSerializer::DeserializeMaterial(const ResourceID& resourceID, const string& data)
	{
		YAML::Node baseNode = YAML::Load(data);
		string name = baseNode["name"].as<string>();
		string shaderPath = baseNode["shader path"].as<string>();

		Ref<Shader> shader = Engine::Get()->GetResourceLibrary().GetOrLoad<Shader>(shaderPath);
		ManagedRef<Material> material = CreateManagedRef<Material>(resourceID, name, shader);

		YAML::Node uniformsNode = baseNode["uniforms"];
		ShaderUniformDataSerializer::Deserialize(uniformsNode, material->_uniformData, material->_textures);

		return material;
	}

	string MaterialSerializer::SerializeMaterialInstance(const MaterialInstance& material)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "name" << YAML::Value << material.GetName();

		Ref<Material> baseMaterial = material._baseMaterial;
		Assert(baseMaterial.IsValid())

		out << YAML::Key << "base material path" << YAML::Value << baseMaterial->GetContentPath();

		out << YAML::Key << "uniforms" << YAML::Value << YAML::BeginMap;

		ShaderUniformDataSerializer::Serialize(out, material._uniformData, material._textures);

		out << YAML::EndMap;

		out << YAML::EndMap << YAML::Comment("Fix");

		return string(out.c_str());
	}

	ManagedRef<Resource> MaterialSerializer::DeserializeMaterialInstance(const ResourceID& resourceID, const string& data)
	{
		YAML::Node baseNode = YAML::Load(data);
		string name = baseNode["name"].as<string>();
		string baseMaterialPath = baseNode["base material path"].as<string>();

		Ref<Material> baseMaterial = Engine::Get()->GetResourceLibrary().GetOrLoad<Material>(baseMaterialPath);
		ManagedRef<MaterialInstance> material = CreateManagedRef<MaterialInstance>(resourceID, name, baseMaterial);

		YAML::Node uniformsNode = baseNode["uniforms"];
		ShaderUniformDataSerializer::Deserialize(uniformsNode, material->_uniformData, material->_textures);

		return material;
	}
}