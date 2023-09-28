#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Matrix.h>
#include "Image.h"
#include "ImageSampler.h"

namespace Coco::Rendering
{
	/// @brief Container for uniform data to pass to shaders
	struct ShaderUniformData
	{
		/// @brief A key for a uniform
		using UniformKey = uint64;

		/// @brief A pair of float values
		using float2 = std::array<float, 2>;

		/// @brief A set of 3 float values
		using float3 = std::array<float, 3>;

		/// @brief A set of 4 float values
		using float4 = std::array<float, 4>;

		/// @brief A pair of int32 values
		using int2 = std::array<int32, 2>;

		/// @brief A set of 3 int32 values
		using int3 = std::array<int32, 3>;

		/// @brief A set of 4 int32 values
		using int4 = std::array<int32, 4>;

		/// @brief A set of 16 float values for a Matrix4x4
		using Mat4x4 = std::array<float, Matrix4x4::CellCount>;

		/// @brief An image and image sampler combination
		using TextureSampler = std::pair<Ref<Image>, Ref<ImageSampler>>;

		/// @brief A special version number used to signify temporary uniform data
		static constexpr uint64 TemporaryVersion = Math::MaxValue<uint64>();

		/// @brief Empty uniform data
		static const ShaderUniformData Empty;

		/// @brief The version of this uniform data
		uint64 Version;

		/// @brief The float uniforms
		std::unordered_map<UniformKey, float> Floats;

		/// @brief The float2 uniforms
		std::unordered_map<UniformKey, float2> Float2s;

		/// @brief The float3 uniforms
		std::unordered_map<UniformKey, float3> Float3s;

		/// @brief The float4 uniforms
		std::unordered_map<UniformKey, float4> Float4s;

		/// @brief The mat4 uniforms
		std::unordered_map<UniformKey, Mat4x4> Mat4x4s;

		/// @brief The int uniforms
		std::unordered_map<UniformKey, int32> Ints;

		/// @brief The int2 uniforms
		std::unordered_map<UniformKey, int2> Int2s;

		/// @brief The int3 uniforms
		std::unordered_map<UniformKey, int3> Int3s;

		/// @brief The int4 uniforms
		std::unordered_map<UniformKey, int4> Int4s;

		/// @brief The bool uniforms
		std::unordered_map<UniformKey, uint8> Bools;

		/// @brief The texture uniforms
		std::unordered_map<UniformKey, TextureSampler> Textures;

		ShaderUniformData();
		ShaderUniformData(uint64 version);

		/// @brief Makes a uniform key from a name
		/// @param name The uniform name
		/// @return The key
		static UniformKey MakeKey(const char* name);

		/// @brief Convertes a Vector2 into a float2
		/// @param v The vector
		/// @return The float2
		constexpr static float2 ToFloat2(const Vector2& v) { return float2{ static_cast<float>(v.X), static_cast<float>(v.Y) }; }

		/// @brief Convertes a Vector3 into a float3
		/// @param v The vector
		/// @return The float3
		constexpr static float3 ToFloat3(const Vector3& v) { return float3{ static_cast<float>(v.X), static_cast<float>(v.Y), static_cast<float>(v.Z) }; }

		/// @brief Convertes a Vector4 into a float4
		/// @param v The vector
		/// @return The float4
		constexpr static float4 ToFloat4(const Vector4& v) { return float4{ static_cast<float>(v.X), static_cast<float>(v.Y), static_cast<float>(v.Z), static_cast<float>(v.W) }; }

		/// @brief Convertes a Matrix4x4 into a Mat4x4
		/// @param v The matrix
		/// @return The Mat4x4
		static Mat4x4 ToMat4x4(const Matrix4x4& v);

		/// @brief Convertes a Vector2Int into a int2
		/// @param v The vector
		/// @return The int2
		constexpr static int2 ToInt2(const Vector2Int& v) { return int2{ v.X, v.Y }; }

		/// @brief Convertes a Vector3Int into a int3
		/// @param v The vector
		/// @return The int3
		constexpr static int3 ToInt3(const Vector3Int& v) { return int3{ v.X, v.Y, v.Z }; }

		/// @brief Convertes a Vector4Int into a int4
		/// @param v The vector
		/// @return The int4
		constexpr static int4 ToInt4(const Vector4Int& v) { return int4{ v.X, v.Y, v.Z, v.W }; }

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