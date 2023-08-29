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

	public:
		Material(const ResourceID& id, const string& name);
		Material(const ResourceID& id, const string& name, Ref<Shader> shader);
		~Material() override;

		DefineResourceType(Material)

		const ResourceID& GetID() const override { return ID; }
		ResourceVersion GetMaterialVersion() const override { return GetVersion(); }
		void SetUniformData(const ShaderUniformData& uniformData) override;
		ShaderUniformData GetUniformData() const override;
		Ref<Shader> GetShader() const noexcept override { return _shader; }

		/// @brief Sets the shader for this material
		/// @param shader The shader
		void SetShader(Ref<Shader> shader);

		/// @brief Sets a int property
		/// @param name The name of the property
		/// @param value The value
		void SetInt(const string& name, int32_t value);

		/// @brief Gets a int property
		/// @param name The name of the property
		/// @return The int, or a default int if no property was found
		int32_t GetInt(const string& name) const;

		/// @brief Gets all the int properties that this material has
		/// @return This material's int properties
		const UnorderedMap<string, int32_t>& GetIntProperties() const noexcept { return _uniformData.Ints; }

		/// @brief Sets a Vector2Int property
		/// @param name The name of the property
		/// @param value The value
		void SetVector2Int(const string& name, const Vector2Int& value);

		/// @brief Gets a Vector2Int property
		/// @param name The name of the property
		/// @return The Vector2Int, or a default Vector2Int if no property was found
		Vector2Int GetVector2Int(const string& name) const;

		/// @brief Gets all the Vector2Int properties that this material has
		/// @return This material's Vector2Int properties
		const UnorderedMap<string, Vector2Int>& GetVector2IntProperties() const noexcept { return _uniformData.Vector2Ints; }

		/// @brief Sets a Vector3Int property
		/// @param name The name of the property
		/// @param value The value
		void SetVector3Int(const string& name, const Vector3Int& value);

		/// @brief Gets a Vector3Int property
		/// @param name The name of the property
		/// @return The Vector3Int, or a default Vector3Int if no property was found
		Vector3Int GetVector3Int(const string& name) const;

		/// @brief Gets all the Vector3Int properties that this material has
		/// @return This material's Vector3Int properties
		const UnorderedMap<string, Vector3Int>& GetVector3IntProperties() const noexcept { return _uniformData.Vector3Ints; }

		/// @brief Sets a Vector4Int property
		/// @param name The name of the property
		/// @param value The value
		void SetVector4Int(const string& name, const Vector4Int& value);

		/// @brief Gets a Vector4Int property
		/// @param name The name of the property
		/// @return The Vector4Int, or a default Vector4Int if no property was found
		Vector4Int GetVector4Int(const string& name) const;

		/// @brief Gets all the Vector4Int properties that this material has
		/// @return This material's Vector4Int properties
		const UnorderedMap<string, Vector4Int>& GetVector4IntProperties() const noexcept { return _uniformData.Vector4Ints; }

		/// @brief Sets a float property
		/// @param name The name of the property
		/// @param value The value
		void SetFloat(const string& name, float value);

		/// @brief Gets a float property
		/// @param name The name of the property
		/// @return The float, or a default float if no property was found
		float GetFloat(const string& name) const;

		/// @brief Gets all the float properties that this material has
		/// @return This material's float properties
		const UnorderedMap<string, float>& GetFloatProperties() const noexcept { return _uniformData.Floats; }

		/// @brief Sets a Vector2 property
		/// @param name The name of the property
		/// @param value The value
		void SetVector2(const string& name, const Vector2& value);

		/// @brief Gets a Vector2 property
		/// @param name The name of the property
		/// @return The Vector2, or a default Vector2 if no property was found
		Vector2 GetVector2(const string& name) const;

		/// @brief Gets all the Vector2 properties that this material has
		/// @return This material's Vector2 properties
		const UnorderedMap<string, Vector2>& GetVector2Properties() const noexcept { return _uniformData.Vector2s; }

		/// @brief Sets a Vector3 property
		/// @param name The name of the property
		/// @param value The value
		void SetVector3(const string& name, const Vector3& value);

		/// @brief Gets a Vector3 property
		/// @param name The name of the property
		/// @return The Vector3, or a default Vector3 if no property was found
		Vector3 GetVector3(const string& name) const;

		/// @brief Gets all the Vector3 properties that this material has
		/// @return This material's Vector3 properties
		const UnorderedMap<string, Vector3>& GetVector3Properties() const noexcept { return _uniformData.Vector3s; }

		/// @brief Sets a Vector4 property
		/// @param name The name of the property
		/// @param value The value
		void SetVector4(const string& name, const Vector4& value);

		/// @brief Gets a Vector4 property
		/// @param name The name of the property
		/// @return The Vector4, or a default Vector4 if no property was found
		Vector4 GetVector4(const string& name) const;

		/// @brief Gets all the Vector4 properties that this material has
		/// @return This material's Vector4 properties
		const UnorderedMap<string, Vector4>& GetVector4Properties() const noexcept { return _uniformData.Vector4s; }

		/// @brief Sets a Color property. Use this for proper color-space conversion
		/// @param name The name of the property
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
		void SetUniformData(const ShaderUniformData& uniformData) override;
		ShaderUniformData GetUniformData() const override;
		Ref<Shader> GetShader() const noexcept override { return _baseMaterial->GetShader(); }

		/// @brief Gets the material that this instance is based on
		/// @return The base material
		Ref<Material> GetBaseMaterial() const { return _baseMaterial; }

		/// @brief Sets a int property
		/// @param name The name of the property
		/// @param value The value
		void SetInt(const string& name, int32_t value);

		/// @brief Gets a int property
		/// @param name The name of the property
		/// @return The int, or a default int if no property was found
		int32_t GetInt(const string& name) const;

		/// @brief Gets all the int properties that this material has
		/// @return This material's int properties
		UnorderedMap<string, int32_t> GetIntProperties() const noexcept;

		/// @brief Sets a Vector2Int property
		/// @param name The name of the property
		/// @param value The value
		void SetVector2Int(const string& name, const Vector2Int& value);

		/// @brief Gets a Vector2Int property
		/// @param name The name of the property
		/// @return The Vector2Int, or a default Vector2Int if no property was found
		Vector2Int GetVector2Int(const string& name) const;

		/// @brief Gets all the Vector2Int properties that this material has
		/// @return This material's Vector2Int properties
		UnorderedMap<string, Vector2Int> GetVector2IntProperties() const noexcept;

		/// @brief Sets a Vector3Int property
		/// @param name The name of the property
		/// @param value The value
		void SetVector3Int(const string& name, const Vector3Int& value);

		/// @brief Gets a Vector3Int property
		/// @param name The name of the property
		/// @return The Vector3Int, or a default Vector3Int if no property was found
		Vector3Int GetVector3Int(const string& name) const;

		/// @brief Gets all the Vector3Int properties that this material has
		/// @return This material's Vector3Int properties
		UnorderedMap<string, Vector3Int> GetVector3IntProperties() const noexcept;

		/// @brief Sets a Vector4Int property
		/// @param name The name of the property
		/// @param value The value
		void SetVector4Int(const string& name, const Vector4Int& value);

		/// @brief Gets a Vector4Int property
		/// @param name The name of the property
		/// @return The Vector4Int, or a default Vector4Int if no property was found
		Vector4Int GetVector4Int(const string& name) const;

		/// @brief Gets all the Vector4Int properties that this material has
		/// @return This material's Vector4Int properties
		UnorderedMap<string, Vector4Int> GetVector4IntProperties() const noexcept;

		/// @brief Sets a float property
		/// @param name The name of the property
		/// @param value The value
		void SetFloat(const string& name, float value);

		/// @brief Gets a float property
		/// @param name The name of the property
		/// @return The float, or a default float if no property was found
		float GetFloat(const string& name) const;

		/// @brief Gets all the float properties that this material has
		/// @return This material's float properties
		UnorderedMap<string, float> GetFloatProperties() const noexcept;

		/// @brief Sets a Vector2 property
		/// @param name The name of the property
		/// @param value The value
		void SetVector2(const string& name, const Vector2& value);

		/// @brief Gets a Vector2 property
		/// @param name The name of the property
		/// @return The Vector2, or a default Vector2 if no property was found
		Vector2 GetVector2(const string& name) const;

		/// @brief Gets all the Vector2 properties that this material has
		/// @return This material's Vector2 properties
		UnorderedMap<string, Vector2> GetVector2Properties() const noexcept;

		/// @brief Sets a Vector3 property
		/// @param name The name of the property
		/// @param value The value
		void SetVector3(const string& name, const Vector3& value);

		/// @brief Gets a Vector3 property
		/// @param name The name of the property
		/// @return The Vector3, or a default Vector3 if no property was found
		Vector3 GetVector3(const string& name) const;

		/// @brief Gets all the Vector3 properties that this material has
		/// @return This material's Vector3 properties
		UnorderedMap<string, Vector3> GetVector3Properties() const noexcept;

		/// @brief Sets a Vector4 property
		/// @param name The name of the property
		/// @param value The value
		void SetVector4(const string& name, const Vector4& value);

		/// @brief Gets a Vector4 property
		/// @param name The name of the property
		/// @return The Vector4, or a default Vector4 if no property was found
		Vector4 GetVector4(const string& name) const;

		/// @brief Gets all the Vector4 properties that this material instance has
		/// @return This material instance's Vector4 properties
		UnorderedMap<string, Vector4> GetVector4Properties() const noexcept;

		/// @brief Sets a Color property. Use this for proper color-space conversion
		/// @param name The name of the property
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
		/// @param name The name of the property
		/// @param texture The texture
		void SetTexture(const string& name, Ref<Texture> texture);

		/// @brief Gets a texture property
		/// @param name The name of the property
		/// @return The texture's ID, or an invalid ID if no property was found
		ResourceID GetTexture(const string& name) const;

		/// @brief Gets all the texture properties that this material instance has
		/// @return This material instance's texture properties
		UnorderedMap<string, ResourceID> GetTextureProperties() const noexcept;
	};
}