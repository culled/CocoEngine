#pragma once

#include "Graphics/ShaderUniformData.h"
#include <Coco/Core/Types/Color.h>
#include "Shader.h"
#include "Texture.h"

namespace Coco::Rendering
{
	/// @brief Base class for all providers of material data
	class MaterialDataProvider
	{
	public:
		virtual ~MaterialDataProvider() = default;

		/// @brief Gets the ID of this material instance
		/// @return The instance ID
		virtual uint64 GetID() const = 0;

		/// @brief Gets the uniform data of this material instance
		/// @return The uniform data
		virtual ShaderUniformData GetUniformData() const = 0;

		/// @brief Gets the shader used by this material instance
		/// @return The shader
		virtual Ref<Shader> GetShader() const = 0;
	};

	/// @brief Describes how to render a surface
	class Material : public MaterialDataProvider
	{
	private:
		Ref<Shader> _shader;
		ShaderUniformData _uniformData;
		std::unordered_map<ShaderUniformData::UniformKey, Ref<Texture>> _textures;

	public:
		Material();
		Material(Ref<Shader> shader);

		uint64 GetID() const final;
		ShaderUniformData GetUniformData() const final { return _uniformData; }
		Ref<Shader> GetShader() const final { return _shader; }

		/// @brief Sets the shader of this material
		/// @param shader The shader
		void SetShader(Ref<Shader> shader);

		/// @brief Sets a float uniform
		/// @param name The name of the uniform
		/// @param value The value
		void SetFloat(const char* name, float value);

		/// @brief Gets a float uniform
		/// @param name The name of the uniform
		/// @return The value
		float GetFloat(const char* name) const;

		/// @brief Sets a float2 uniform
		/// @param name The name of the uniform
		/// @param value The value
		void SetFloat2(const char* name, const Vector2& value);

		/// @brief Gets a float2 uniform
		/// @param name The name of the uniform
		/// @return The value
		Vector2 GetFloat2(const char* name) const;

		/// @brief Sets a float3 uniform
		/// @param name The name of the uniform
		/// @param value The value
		void SetFloat3(const char* name, const Vector3& value);

		/// @brief Gets a float3 uniform
		/// @param name The name of the uniform
		/// @return The value
		Vector3 GetFloat3(const char* name) const;

		/// @brief Sets a float4 uniform
		/// @param name The name of the uniform
		/// @param value The value
		void SetFloat4(const char* name, const Vector4& value);

		/// @brief Sets a float4 uniform from a color
		/// @param name The name of the uniform
		/// @param value The color
		/// @param asLinear If true, the color will be sent to the shader converted to a linear value
		void SetFloat4(const char* name, const Color& value, bool asLinear = true);

		/// @brief Gets a float4 uniform
		/// @param name The name of the uniform
		/// @return The value 
		Vector4 GetFloat4(const char* name) const;

		/// @brief Sets a matrix4x4 uniform
		/// @param name The name of the uniform
		/// @param value The value
		void SetMatrix4x4(const char* name, const Matrix4x4& value);

		/// @brief Gets a matrix4x4 uniform
		/// @param name The name of the uniform
		/// @return The value 
		Matrix4x4 GetMatrix4x4(const char* name) const;

		/// @brief Sets an int uniform
		/// @param name The name of the uniform
		/// @param value The value
		void SetInt(const char* name, int32 value);

		/// @brief Gets an int uniform
		/// @param name The name of the uniform
		/// @return The value 
		int32 GetInt(const char* name) const;

		/// @brief Sets an int2 uniform
		/// @param name The name of the uniform
		/// @param value The value
		void SetInt2(const char* name, const Vector2Int& value);

		/// @brief Gets an int2 uniform
		/// @param name The name of the uniform
		/// @return The value 
		Vector2Int GetInt2(const char* name) const;

		/// @brief Sets an int3 uniform
		/// @param name The name of the uniform
		/// @param value The value
		void SetInt3(const char* name, const Vector3Int& value);

		/// @brief Gets an int3 uniform
		/// @param name The name of the uniform
		/// @return The value
		Vector3Int GetInt3(const char* name) const;

		/// @brief Sets an int4 uniform
		/// @param name The name of the uniform
		/// @param value The value
		void SetInt4(const char* name, const Vector4Int& value);

		/// @brief Gets an int4 uniform
		/// @param name The name of the uniform
		/// @return The value
		Vector4Int GetInt4(const char* name) const;

		/// @brief Sets a bool uniform
		/// @param name The name of the uniform
		/// @param value The value 
		void SetBool(const char* name, bool value);

		/// @brief Gets a bool uniform
		/// @param name The name of the uniform
		/// @return The value
		bool GetBool(const char* name) const;

		/// @brief Sets a texture uniform
		/// @param name The name of the uniform
		/// @param texture The texture
		void SetTexture(const char* name, Ref<Texture> texture);

		/// @brief Gets a texture uniform
		/// @param name The name of the uniform
		/// @return The texture
		Ref<Texture> GetTexture(const char* name) const;

	private:
		/// @brief Increments the version of this material
		void IncrementVersion();
	};
}