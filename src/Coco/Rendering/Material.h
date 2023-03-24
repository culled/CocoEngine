#pragma once

#include "RenderingResource.h"

#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Map.h>
#include "Graphics/Resources/BufferTypes.h"
#include "Shader.h"
#include "Texture.h"

namespace Coco::Rendering
{
	class MaterialInstance;

	/// @brief A binding for a subshader to a material
	struct SubshaderUniformBinding
	{
		/// @brief The offset of the uniform (in bytes)
		uint64_t Offset;
		
		/// @brief The size of the binding (in bytes)
		uint64_t Size = 0;

		SubshaderUniformBinding(uint64_t offset) :
			Offset(offset), Size(0) {}
	};

	/// @brief Holds properties for a shader for modifying the look of rendered geometry
	class COCOAPI Material : public RenderingResource
	{
		friend MaterialInstance;

	public:
		/// @brief The size of a Vector4 uniform
		static constexpr uint Vector4Size = GetBufferDataFormatSize(BufferDataFormat::Vector4);

	protected:
		/// @brief The shader that this material uses
		Ref<Shader> Shader;

		/// @brief The Vector4 properties of this material
		Map<string, Vector4> Vector4Properties;

		/// @brief The texture properties of this material
		Map<string, Ref<Texture>> TextureProperties;

	private:
		ResourceVersion _propertyMapVersion = 0;
		Map<string, SubshaderUniformBinding> _subshaderBindings;
		List<uint8_t> _bufferData;
		bool _isBufferDataDirty = true;

	public:
		Material(Ref<Rendering::Shader> shader, const string& name = "");
		virtual ~Material() override;

		/// @brief Gets the shader that this material uses
		/// @return The shader that this material uses
		Ref<Rendering::Shader> GetShader() const noexcept { return Shader; }

		/// @brief Sets a Vector4 property
		/// @param name The name of the Vector4 property
		/// @param value The value
		void SetVector4(const string& name, const Vector4& value);

		/// @brief Gets a Vector4 property
		/// @param name The name of the Vector4 property
		/// @return The Vector4, or a default Vector4 if no property was found
		Vector4 GetVector4(const string& name) const;

		/// @brief Gets all the Vector4 properties that this material has
		/// @return This material's Vector4 properties
		const Map<string, Vector4>& GetVector4Properties() const noexcept { return Vector4Properties; }

		/// @brief Sets a texture property
		/// @param name The name of the texture property
		/// @param texture The texture
		void SetTexture(const string& name, Ref<Texture> texture);

		/// @brief Gets a texture property
		/// @param name The name of the texture property
		/// @return The texture, or a null reference if no property was found
		Ref<Texture> GetTexture(const string& name) const;

		/// @brief Gets all the texture properties that this material has
		/// @return This material's texture properties
		const Map<string, Ref<Texture>>& GetTextureProperties() const noexcept { return TextureProperties; }

		/// @brief Gets uniform buffer data from this material's currently set properties
		/// @return This material's properties as a byte array
		const List<uint8_t>& GetBufferData();

		/// @brief Tries to get a binding for a subshader with a given name
		/// @param subshaderName The name of the subshader
		/// @param binding Will be filled with a pointer to the binding if found
		/// @return True if the binding was found
		bool TryGetSubshaderBinding(const string& subshaderName, const SubshaderUniformBinding*& binding);

	private:
		/// @brief Updates the property maps from this material's shader
		/// @param forceUpdate If true, the map will be updated regardless if it matches the version of the shader
		void UpdatePropertyMaps(bool forceUpdate);

		/// @brief Updates this material's buffer data from its currently set properties
		void UpdateBufferData();
	};
}