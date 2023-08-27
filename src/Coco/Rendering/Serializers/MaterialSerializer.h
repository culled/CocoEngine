#pragma once

#include <Coco/Core/Resources/Serializers/KeyValueResourceSerializer.h>

#include <Coco/Core/Types/Map.h>
#include "../Material.h"

namespace Coco
{
	struct Vector4;
}

namespace Coco::Rendering
{
	/// @brief A serializer for material files (*.cmaterial)
	class MaterialSerializer final : public KeyValueResourceSerializer
	{
	private:
		static constexpr const char* s_materialIDVariable = "id";
		static constexpr const char* s_materialNameVariable = "name";
		static constexpr const char* s_materialShaderVariable = "shader";
		static constexpr const char* s_propertiesSection = "properties";
		static constexpr const char* s_vector4Section = "vector4";
		static constexpr const char* s_colorSection = "color";
		static constexpr const char* s_textureSection = "texture";

	public:
		MaterialSerializer() = default;
		~MaterialSerializer() final = default;

		DefineSerializerResourceType(Material)

		string Serialize(ResourceLibrary& library, const Ref<Resource>& resource) final;
		ManagedRef<Resource> Deserialize(ResourceLibrary& library, const string& data) final;

	private:
		/// @brief Reads the properties section for a material
		/// @param reader The reader
		/// @param vec4Properties Will be filled with the Vector4 properties
		/// @param colorProperties Will be filled with the Color properties
		/// @param textureProperties Will be filled with the texture properties
		void ReadPropertiesSection(
			KeyValueReader& reader, 
			UnorderedMap<string, Vector4>& vec4Properties, 
			UnorderedMap<string, Color>& colorProperties,
			UnorderedMap<string, string>& textureProperties);

		/// @brief Reads the Vector4 properties section for a material
		/// @param reader The reader
		/// @param vec4Properties Will be filled with the Vector4 properties
		void ReadVector4Section(KeyValueReader& reader, UnorderedMap<string, Vector4>& vec4Properties);

		/// @brief Reads the Color properties section for a material
		/// @param reader The reader
		/// @param colorProperties Will be filled with the Color properties
		void ReadColorSection(KeyValueReader& reader, UnorderedMap<string, Color>& colorProperties);

		/// @brief Reads the texture properties section for a material
		/// @param reader The reader
		/// @param textureProperties Will be filled with the texture properties
		void ReadTextureSection(KeyValueReader& reader, UnorderedMap<string, string>& textureProperties);
	};
}