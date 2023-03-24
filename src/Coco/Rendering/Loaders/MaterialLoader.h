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

	/// @brief A loader for material files (*.cmaterial)
	class MaterialLoader final : public KeyValueResourceLoader
	{
	private:
		static constexpr const char* s_materialNameVariable = "name";
		static constexpr const char* s_materialShaderVariable = "shader";
		static constexpr const char* s_propertiesSection = "properties";
		static constexpr const char* s_vector4Section = "vector4";
		static constexpr const char* s_textureSection = "texture";

	public:
		MaterialLoader(ResourceLibrary* library);
		~MaterialLoader() final = default;

		string GetResourceTypename() const noexcept final { return ResourceTypeToString(ResourceType::Material); }

	protected:
		Ref<Resource> LoadImpl(const string& path) final;
		void SaveImpl(const Ref<Resource>& resource, const string& path) final;

	private:
		/// @brief Reads the properties section for a material
		/// @param reader The reader
		/// @param vec4Properties Will be filled with the Vector4 properties
		/// @param textureProperties Will be filled with the texture properties
		void ReadPropertiesSection(KeyValueReader& reader, Map<string, Vector4>& vec4Properties, Map<string, string>& textureProperties);

		/// @brief Reads the Vector4 properties section for a material
		/// @param reader The reader
		/// @param vec4Properties Will be filled with the Vector4 properties
		void ReadVector4Section(KeyValueReader& reader, Map<string, Vector4>& vec4Properties);

		/// @brief Reads the texture properties section for a material
		/// @param reader The reader
		/// @param textureProperties Will be filled with the texture properties
		void ReadTextureSection(KeyValueReader& reader, Map<string, string>& textureProperties);
	};
}