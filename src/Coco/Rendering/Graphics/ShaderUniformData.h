#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Matrix.h>
#include "Image.h"
#include "ImageSampler.h"

namespace Coco::Rendering
{
	/// @brief A union for shader uniform data
	struct ShaderUniformUnion
	{
		/// @brief The maximum data size
		static constexpr int MaxDataSize = 16 * sizeof(float);

		union
		{
			bool AsBool;
			float AsFloat;
			float AsFloat2[2];
			float AsFloat3[3];
			float AsFloat4[4];
			float AsMat4x4[Matrix4x4::CellCount];
			int AsInt;
			int AsInt2[2];
			int AsInt3[3];
			int AsInt4[4];
			uint8 AsData[MaxDataSize];
		};

		ShaderUniformUnion();
		ShaderUniformUnion(bool value);
		ShaderUniformUnion(float value);
		ShaderUniformUnion(const Vector2& value);
		ShaderUniformUnion(const Vector3& value);
		ShaderUniformUnion(const Vector4& value);
		ShaderUniformUnion(const Color& value);
		ShaderUniformUnion(int value);
		ShaderUniformUnion(const Vector2Int& value);
		ShaderUniformUnion(const Vector3Int& value);
		ShaderUniformUnion(const Vector4Int& value);
		ShaderUniformUnion(const Matrix4x4& value);

		/// @brief Gets the contained value as a certain type
		/// @tparam ValueType The type of value to return
		/// @return The value
		template<typename ValueType>
		ValueType As() const;

		/// @brief Gets the value as a boolean
		/// @return The value
		template<>
		bool As<bool>() const { return AsBool; }

		/// @brief Gets the value as a float
		/// @return The value
		template<>
		float As<float>() const { return AsFloat; }

		/// @brief Gets the value as a Vector2
		/// @return The value
		template<>
		Vector2 As<Vector2>() const;

		/// @brief Gets the value as a Vector3
		/// @return The value
		template<>
		Vector3 As<Vector3>() const;
		
		/// @brief Gets the value as a Vector4
		/// @return The value
		template<>
		Vector4 As<Vector4>() const;

		/// @brief Gets the value as a Color
		/// @return The value
		template<>
		Color As<Color>() const;

		/// @brief Gets the value as a int
		/// @return The value
		template<>
		int As<int>() const { return AsInt; }

		/// @brief Gets the value as a Vector2Int
		/// @return The value
		template<>
		Vector2Int As<Vector2Int>() const;

		/// @brief Gets the value as a Vector3Int
		/// @return The value
		template<>
		Vector3Int As<Vector3Int>() const;

		/// @brief Gets the value as a Vector4Int
		/// @return The value
		template<>
		Vector4Int As<Vector4Int>() const;

		/// @brief Gets the value as a Matrix4x4
		/// @return The value
		template<>
		Matrix4x4 As<Matrix4x4>() const;
	};

	/// @brief Container for uniform data to pass to shaders
	struct ShaderUniformData
	{
		/// @brief A key for a uniform
		using UniformKey = uint64;

		/// @brief An image and image sampler combination
		using TextureSampler = std::pair<Ref<Image>, Ref<ImageSampler>>;

		/// @brief A special version number used to signify temporary uniform data
		static constexpr uint64 TemporaryVersion = Math::MaxValue<uint64>();

		/// @brief Empty uniform data
		static const ShaderUniformData Empty;

		/// @brief The version of this uniform data
		uint64 Version;

		/// @brief The uniforms
		std::unordered_map<UniformKey, ShaderUniformUnion> Uniforms;

		/// @brief The texture uniforms
		std::unordered_map<UniformKey, TextureSampler> Textures;

		ShaderUniformData();
		ShaderUniformData(uint64 version);

		/// @brief Makes a uniform key from a name
		/// @param name The uniform name
		/// @return The key
		static UniformKey MakeKey(const char* name);

		/// @brief Makes a uniform key from a name
		/// @param name The uniform name
		/// @return The key
		static UniformKey MakeKey(const string& name);

		/// @brief Converts an Image and ImageSampler to a TextureSampler
		/// @param image The image
		/// @param sampler The image sampler
		/// @return The TextureSampler
		static TextureSampler ToTextureSampler(const Ref<Image>& image, const Ref<ImageSampler>& sampler) { return std::make_pair(image, sampler); }

		/// @brief Clears all uniform data
		void Clear();

		/// @brief Overwrites uniform data with uniforms from another object
		/// @param other The other uniform data
		void MergeWith(const ShaderUniformData& other);
	};
}