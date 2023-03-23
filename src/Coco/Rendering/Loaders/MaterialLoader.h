#pragma once

#include <Coco/Core/Resources/Loaders/KeyValueResourceLoader.h>

#include <Coco/Core/Types/Map.h>

namespace Coco
{
	class File;
	struct Vector4;
}

namespace Coco::Rendering
{
	class Texture;

	/// <summary>
	/// A loader for material files (*.cmaterial)
	/// </summary>
	class MaterialLoader final : public KeyValueResourceLoader
	{
	private:
		static constexpr const char* s_materialNameVariable = "name";
		static constexpr const char* s_materialShaderVariable = "shader";
		static constexpr const char* s_propertiesSection = "properties";
		static constexpr const char* s_vector4Section = "vector4";
		static constexpr const char* s_textureSection = "texture";

	public:
		MaterialLoader(ResourceLibrary* library, const string& basePath);
		~MaterialLoader() final = default;

		const char* GetResourceTypename() const noexcept final { return ResourceTypeToString(ResourceType::Material); }

	protected:
		Ref<Resource> LoadImpl(const string& path) final;
		void SaveImpl(const Ref<Resource>& resource, const string& path) final;

	private:
		void ReadPropertiesSection(KeyValueReader& reader, Map<string, Vector4>& vec4Properties, Map<string, string>& textureProperties);
		void ReadVector4Section(KeyValueReader& reader, Map<string, Vector4>& vec4Properties);
		void ReadTextureSection(KeyValueReader& reader, Map<string, string>& textureProperties);
	};
}