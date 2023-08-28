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
		static constexpr const char* s_uniformsSection = "uniforms";
		static constexpr const char* s_uniformsTypeVariable = "type";
		static constexpr const char* s_uniformsValueVariable = "value";
		static constexpr int s_textureType = 10;

	public:
		MaterialSerializer() = default;
		~MaterialSerializer() final = default;

		DefineSerializerResourceType(Material)

		string Serialize(ResourceLibrary& library, const Ref<Resource>& resource) final;
		ManagedRef<Resource> Deserialize(ResourceLibrary& library, const string& data) final;

	private:
		/// @brief Reads the properties section for a material
		/// @param reader The reader
		/// @param library The resource library
		/// @param uniforms Will be filled with uniforms
		void ReadUniformsSection(
			KeyValueReader& reader, 
			ResourceLibrary& library,
			ShaderUniformData& uniforms);
	};
}