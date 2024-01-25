#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Defines.h>
#include "../ShaderTypes.h"
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/String.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Matrix.h>

namespace Coco::Rendering
{
	class GraphicsDevice;
	class Texture;

	/// @brief Scopes for a shader uniform
	enum class UniformScope
	{
		/// @brief Can only be updated once per render operation
		Global,

		/// @brief Can only be updated once per shader per render operation
		ShaderGlobal,

		/// @brief Can only be updated once per instance
		Instance,

		/// @brief Can be updated every draw call
		Draw
	};

	/// @brief Types of shader uniforms
	enum class ShaderUniformType
	{
		Float,
		Float2,
		Float3,
		Float4,
		Color,
		Matrix4x4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool,
		Texture
	};

	/// @brief Types of default textures
	enum class DefaultTextureType
	{
		White,
		FlatNormal,
		Checkered
	};

	using Float2 = std::array<float, 2>;
	using Float3 = std::array<float, 3>;
	using Float4 = std::array<float, 4>;
	using Int2 = std::array<int, 2>;
	using Int3 = std::array<int, 3>;
	using Int4 = std::array<int, 4>;
	using FloatMatrix4x4 = std::array<float, 16>;

	/// @brief Gets the buffer data type for a data uniform
	/// @param dataUniform The type of data uniform
	/// @return The buffer data type of the data uniform
	BufferDataType GetBufferDataType(ShaderUniformType dataUniform);

	/// @brief Determines if the given uniform type is a data uniform (not a texture uniform)
	/// @return True if the given uniform type holds data
	bool IsDataShaderUniformType(ShaderUniformType type);

	/// @brief Gets the string representation of a ShaderUniformType
	/// @param type The uniform type
	/// @return The string representation
	const char* GetShaderUniformTypeString(ShaderUniformType type);

	/// @brief A uniform for a shader
	struct ShaderUniform
	{
		/// @brief The uniform name (used for referencing from materials)
		string Name;

		/// @brief The type of this uniform
		ShaderUniformType Type;

		/// @brief The points in the rendering stage when this uniform should be bound
		ShaderStageFlags BindingPoints;

		/// @brief The default value for this uniform
		std::any DefaultValue;

		uint32 Order;
		uint64 Offset;
		uint64 DataSize;

		ShaderUniform(const string& name, ShaderUniformType type, ShaderStageFlags bindPoints, std::any defaultValue);
		ShaderUniform(const string& name, ShaderStageFlags bindPoints, float defaultValue);
		ShaderUniform(const string& name, ShaderStageFlags bindPoints, const Vector2& defaultValue);
		ShaderUniform(const string& name, ShaderStageFlags bindPoints, const Vector3& defaultValue);
		ShaderUniform(const string& name, ShaderStageFlags bindPoints, const Vector4& defaultValue);
		ShaderUniform(const string& name, ShaderStageFlags bindPoints, const Color& defaultValue);
		ShaderUniform(const string& name, ShaderStageFlags bindPoints, int defaultValue);
		ShaderUniform(const string& name, ShaderStageFlags bindPoints, const Vector2Int& defaultValue);
		ShaderUniform(const string& name, ShaderStageFlags bindPoints, const Vector3Int& defaultValue);
		ShaderUniform(const string& name, ShaderStageFlags bindPoints, const Vector4Int& defaultValue);
		ShaderUniform(const string& name, ShaderStageFlags bindPoints, const Matrix4x4& defaultValue);
		ShaderUniform(const string& name, ShaderStageFlags bindPoints, bool defaultValue);
		ShaderUniform(const string& name, ShaderStageFlags bindPoints, DefaultTextureType defaultValue);
		ShaderUniform(const string& name, ShaderStageFlags bindPoints, SharedRef<Texture> defaultValue);

		virtual ~ShaderUniform() = default;

		bool operator==(const ShaderUniform& other) const;
	};

	struct ShaderUniformValue
	{
		string Name;

		/// @brief The type of this uniform
		ShaderUniformType Type;

		std::any Value;

		ShaderUniformValue(const string& name, ShaderUniformType type, std::any value);
		ShaderUniformValue(const ShaderUniform& uniform);
		ShaderUniformValue(const string& name, float value);
		ShaderUniformValue(const string& name, const Vector2& value);
		ShaderUniformValue(const string& name, const Vector3& value);
		ShaderUniformValue(const string& name, const Vector4& value);
		ShaderUniformValue(const string& name, const Color& value);
		ShaderUniformValue(const string& name, int value);
		ShaderUniformValue(const string& name, const Vector2Int& value);
		ShaderUniformValue(const string& name, const Vector3Int& value);
		ShaderUniformValue(const string& name, const Vector4Int& value);
		ShaderUniformValue(const string& name, const Matrix4x4& value);
		ShaderUniformValue(const string& name, bool value);
		ShaderUniformValue(const string& name, SharedRef<Texture> value);

		/// @brief Gets the contained value as a certain type
		/// @tparam ValueType The type of value to return
		/// @return The value
		template<typename ValueType>
		const ValueType& As() const;

		/// @brief Gets the value as a boolean
		/// @return The value
		template<>
		const bool& As<bool>() const;

		/// @brief Gets the value as a float
		/// @return The value
		template<>
		const float& As<float>() const;

		/// @brief Gets the value as a Float2
		/// @return The value
		template<>
		const Float2& As<Float2>() const;

		/// @brief Gets the value as a Float3
		/// @return The value
		template<>
		const Float3& As<Float3>() const;

		/// @brief Gets the value as a Float4
		/// @return The value
		template<>
		const Float4& As<Float4>() const;

		/// @brief Gets the value as a int
		/// @return The value
		template<>
		const int& As<int>() const;

		/// @brief Gets the value as a Int2
		/// @return The value
		template<>
		const Int2& As<Int2>() const;

		/// @brief Gets the value as a Int3
		/// @return The value
		template<>
		const Int3& As<Int3>() const;

		/// @brief Gets the value as a Int4
		/// @return The value
		template<>
		const Int4& As<Int4>() const;

		/// @brief Gets the value as a FloatMatrix4x4
		/// @return The value
		template<>
		const FloatMatrix4x4& As<FloatMatrix4x4>() const;

		/// @brief Gets the value as a texture
		/// @return The value
		template<>
		const SharedRef<Texture>& As<SharedRef<Texture>>() const;
	};

	std::vector<uint8> GetDataUniformBytes(const ShaderUniformValue& uniform);
	void GetDataUniformBytes(const ShaderUniformValue& uniform, std::span<uint8> data);
	SharedRef<Texture> GetTextureUniformValue(const ShaderUniformValue& uniform);
}