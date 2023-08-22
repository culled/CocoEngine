#pragma once

#include "RenderingResource.h"

#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Map.h>
#include "Graphics/Resources/BufferTypes.h"
#include "IMaterial.h"
#include "Shader.h"
#include "Texture.h"

namespace Coco::Rendering
{
	/// @brief Holds properties for a shader for modifying the look of rendered geometry
	class COCOAPI Material : public RenderingResource, public IMaterial
	{
		friend class MaterialInstance;

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

		const ResourceID& GetID() const override { return ID; }
		ResourceVersion GetMaterialVersion() const override { return GetVersion(); }
		ShaderUniformData GetUniformData() const override;
		Ref<Shader> GetShader() const noexcept override { return _shader; }

		/// @brief Sets the shader for this material
		/// @param shader The shader
		void SetShader(Ref<Shader> shader);

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

	private:
		/// @brief Updates the property maps from this material's shader
		/// @param forceUpdate If true, the map will be updated regardless if it matches the version of the shader
		void UpdatePropertyMaps(bool forceUpdate);
	};

	/// @brief An instance of a material that can override parameters of the material its based on
	class COCOAPI MaterialInstance : public RenderingResource, public IMaterial
	{
	protected:
		Ref<Material> _baseMaterial;
		ShaderUniformData _uniformData;

	public:
		MaterialInstance(const ResourceID& id, const string& name, Ref<Material> baseMaterial);
		virtual ~MaterialInstance() = default;

		DefineResourceType(MaterialInstance)

		const ResourceID& GetID() const override { return ID; }
		ResourceVersion GetMaterialVersion() const override;
		ShaderUniformData GetUniformData() const override;
		Ref<Shader> GetShader() const noexcept override { return _baseMaterial->GetShader(); }

		/// @brief Gets the material that this instance is based on
		/// @return The base material
		Ref<Material> GetBaseMaterial() const { return _baseMaterial; }

		/// @brief Sets a Vector4 property
		/// @param name The name of the Vector4 property
		/// @param value The value
		void SetVector4(const string& name, const Vector4& value);

		/// @brief Gets a Vector4 property
		/// @param name The name of the Vector4 property
		/// @return The Vector4, or a default Vector4 if no property was found
		Vector4 GetVector4(const string& name) const;

		/// @brief Gets all the Vector4 properties that this material instance has
		/// @return This material instance's Vector4 properties
		UnorderedMap<string, Vector4> GetVector4Properties() const noexcept;

		/// @brief Sets a Color property. Use this for proper color-space conversion
		/// @param name The name of the Color property
		/// @param value The value
		void SetColor(const string& name, const Color& value);

		/// @brief Gets a Color property
		/// @param name The name of the property
		/// @return The color value
		Color GetColor(const string name) const;

		/// @brief Gets all the Color properties that this material instance has
		/// @return This material instances's Color properties
		UnorderedMap<string, Color> GetColorProperties() const noexcept;

		/// @brief Sets a texture property
		/// @param name The name of the texture property
		/// @param texture The texture
		void SetTexture(const string& name, Ref<Texture> texture);

		/// @brief Gets a texture property
		/// @param name The name of the texture property
		/// @return The texture's ID, or an invalid ID if no property was found
		ResourceID GetTexture(const string& name) const;

		/// @brief Gets all the texture properties that this material instance has
		/// @return This material instance's texture properties
		UnorderedMap<string, ResourceID> GetTextureProperties() const noexcept;
	};
}