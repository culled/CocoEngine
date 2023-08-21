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

	/// @brief Holds properties for a shader for modifying the look of rendered geometry
	class COCOAPI Material : public RenderingResource
	{
		friend MaterialInstance;

	protected:
		/// @brief The shader that this material uses
		Ref<Shader> _shader;

		ShaderUniformData _uniformData;

	private:
		ResourceVersion _propertyMapVersion = 0;

	public:
		Material(const ResourceID& id, const string& name);
		Material(const ResourceID& id, const string& name, Ref<Shader> shader);
		~Material() override;

		DefineResourceType(Material)

		/// @brief Sets the shader for this material
		/// @param shader The shader
		void SetShader(Ref<Shader> shader);

		/// @brief Gets the shader that this material uses
		/// @return The shader that this material uses
		Ref<Shader> GetShader() const noexcept { return _shader; }

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
		const UnorderedMap<string, Vector4>& GetVector4Properties() const noexcept { return _uniformData.Vector4s; }

		/// @brief Sets a Color property. Use this for proper color-space conversion
		/// @param name The name of the Color property
		/// @param value The value
		void SetColor(const string& name, const Color& value);

		/// @brief Gets a Color property
		/// @param name The name of the property
		/// @return The color value
		Color GetColor(const string name) const;

		/// @brief Gets all the Color properties that this material has
		/// @return This material's Color properties
		const UnorderedMap<string, Color>& GetColorProperties() const noexcept { return _uniformData.Colors; }

		/// @brief Sets a texture property
		/// @param name The name of the texture property
		/// @param texture The texture
		void SetTexture(const string& name, Ref<Texture> texture);

		/// @brief Gets a texture property
		/// @param name The name of the texture property
		/// @return The texture's ID, or an invalid ID if no property was found
		ResourceID GetTexture(const string& name) const;

		/// @brief Gets all the texture properties that this material has
		/// @return This material's texture properties
		const UnorderedMap<string, ResourceID>& GetTextureProperties() const noexcept { return _uniformData.Textures; }

		const ShaderUniformData& GetUniformData() const { return _uniformData; }

	private:
		/// @brief Updates the property maps from this material's shader
		/// @param forceUpdate If true, the map will be updated regardless if it matches the version of the shader
		void UpdatePropertyMaps(bool forceUpdate);
	};
}