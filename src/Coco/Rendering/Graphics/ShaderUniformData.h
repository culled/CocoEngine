#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Vector.h>
#include "Image.h"
#include "ImageSampler.h"

namespace Coco::Rendering
{
	/// @brief Scopes for a shader uniform
	enum class UniformScope
	{
		/// @brief Can only be updated once per render operation
		Global,

		/// @brief Can only be updated once per instance
		Instance,

		/// @brief Can be updated every draw call
		Draw
	};

	/// @brief Container for uniform data to pass to shaders
	struct ShaderUniformData
	{
		using UniformKey = uint64;

		using float2 = std::array<float, 2>;
		using float3 = std::array<float, 3>;
		using float4 = std::array<float, 4>;

		using int2 = std::array<int32, 2>;
		using int3 = std::array<int32, 3>;
		using int4 = std::array<int32, 4>;

		using Mat4x4 = std::array<float, 16>;

		using TextureSampler = std::pair<Ref<Image>, Ref<ImageSampler>>;

		static const ShaderUniformData Empty;

		std::unordered_map<UniformKey, float> Floats;
		std::unordered_map<UniformKey, float2> Float2s;
		std::unordered_map<UniformKey, float3> Float3s;
		std::unordered_map<UniformKey, float4> Float4s;

		std::unordered_map<UniformKey, Mat4x4> Mat4x4s;

		std::unordered_map<UniformKey, int32> Ints;
		std::unordered_map<UniformKey, int2> Int2s;
		std::unordered_map<UniformKey, int3> Int3s;
		std::unordered_map<UniformKey, int4> Int4s;

		std::unordered_map<UniformKey, uint8> Bools;

		std::unordered_map<UniformKey, TextureSampler> Textures;

		ShaderUniformData();

		static UniformKey MakeKey(const char* name);

		constexpr static float2 ToFloat2(const Vector2& v) { return float2{ static_cast<float>(v.X), static_cast<float>(v.Y) }; }
		constexpr static float3 ToFloat3(const Vector3& v) { return float3{ static_cast<float>(v.X), static_cast<float>(v.Y), static_cast<float>(v.Z) }; }
		constexpr static float4 ToFloat4(const Vector4& v) { return float4{ static_cast<float>(v.X), static_cast<float>(v.Y), static_cast<float>(v.Z), static_cast<float>(v.W) }; }

		constexpr static int2 ToInt2(const Vector2Int& v) { return int2{ v.X, v.Y }; }
		constexpr static int3 ToInt3(const Vector3Int& v) { return int3{ v.X, v.Y, v.Z }; }
		constexpr static int4 ToInt4(const Vector4Int& v) { return int4{ v.X, v.Y, v.Z, v.W }; }

		static TextureSampler ToTextureSampler(const Ref<Image>& image, const Ref<ImageSampler>& sampler) { return std::make_pair(image, sampler); }

		void Clear();
	};
}