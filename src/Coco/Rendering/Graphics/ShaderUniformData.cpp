#include "Renderpch.h"
#include "ShaderUniformData.h"
#include "ShaderUniformTypes.h"

namespace Coco::Rendering
{
	ShaderUniformUnion::ShaderUniformUnion()
	{}

	ShaderUniformUnion::ShaderUniformUnion(bool value) :
		AsBool(value)
	{}

	ShaderUniformUnion::ShaderUniformUnion(float value) :
		AsFloat(value)
	{}

	ShaderUniformUnion::ShaderUniformUnion(const Vector2& value) :
		AsFloat2{static_cast<float>(value.X), static_cast<float>(value.Y)}
	{}

	ShaderUniformUnion::ShaderUniformUnion(const Vector3& value) :
		AsFloat3{ static_cast<float>(value.X), static_cast<float>(value.Y), static_cast<float>(value.Z) }
	{}

	ShaderUniformUnion::ShaderUniformUnion(const Vector4& value) :
		AsFloat4{ static_cast<float>(value.X), static_cast<float>(value.Y), static_cast<float>(value.Z), static_cast<float>(value.W) }
	{}

	ShaderUniformUnion::ShaderUniformUnion(const Color& value) :
		AsFloat4{ 0.f }
	{
		Color c = value.AsLinear();
		AsFloat4[0] = static_cast<float>(c.R);
		AsFloat4[1] = static_cast<float>(c.G);
		AsFloat4[2] = static_cast<float>(c.B);
		AsFloat4[3] = static_cast<float>(c.A);
	}

	ShaderUniformUnion::ShaderUniformUnion(int value) :
		AsInt(value)
	{}

	ShaderUniformUnion::ShaderUniformUnion(const Vector2Int& value) :
		AsInt2{value.X, value.Y}
	{}

	ShaderUniformUnion::ShaderUniformUnion(const Vector3Int& value) :
		AsInt3{ value.X, value.Y, value.Z }
	{}

	ShaderUniformUnion::ShaderUniformUnion(const Vector4Int& value) :
		AsInt4{ value.X, value.Y, value.Z, value.W }
	{}

	ShaderUniformUnion::ShaderUniformUnion(const Matrix4x4& value) :
		AsMat4x4{ 0.0f }
	{
		for (int i = 0; i < Matrix4x4::CellCount; i++)
		{
			AsMat4x4[i] = static_cast<float>(value.Data[i]);
		}
	}

	template<>
	Vector2 ShaderUniformUnion::As() const
	{
		return Vector2(AsFloat2[0], AsFloat2[1]);
	}

	template<>
	Vector3 ShaderUniformUnion::As() const
	{
		return Vector3(AsFloat3[0], AsFloat3[1], AsFloat3[2]);
	}

	template<>
	Vector4 ShaderUniformUnion::As() const
	{
		return Vector4(AsFloat4[0], AsFloat4[1], AsFloat4[2], AsFloat4[3]);
	}

	template<>
	Color ShaderUniformUnion::As() const
	{
		Color c(AsFloat4[0], AsFloat4[1], AsFloat4[2], AsFloat4[3], true);
		return c.AsGamma();
	}

	template<>
	Vector2Int ShaderUniformUnion::As() const
	{
		return Vector2Int(AsInt2[0], AsInt2[1]);
	}

	template<>
	Vector3Int ShaderUniformUnion::As() const
	{
		return Vector3Int(AsInt3[0], AsInt3[1], AsInt3[2]);
	}

	template<>
	Vector4Int ShaderUniformUnion::As() const
	{
		return Vector4Int(AsInt4[0], AsInt4[1], AsInt4[2], AsInt4[3]);
	}

	template<>
	Matrix4x4 ShaderUniformUnion::As() const
	{
		return Matrix4x4({
			AsMat4x4[0], AsMat4x4[1], AsMat4x4[2], AsMat4x4[3],
			AsMat4x4[4], AsMat4x4[5], AsMat4x4[6], AsMat4x4[7],
			AsMat4x4[8], AsMat4x4[9], AsMat4x4[10], AsMat4x4[11],
			AsMat4x4[12], AsMat4x4[13], AsMat4x4[14], AsMat4x4[16]
			});
	}

	const ShaderUniformData ShaderUniformData::Empty = ShaderUniformData();

	ShaderUniformData::ShaderUniformData() :
		ShaderUniformData(TemporaryVersion)
	{}

	ShaderUniformData::ShaderUniformData(uint64 version) :
		Version(version),
		Uniforms{},
		Textures{}
	{}

	std::hash<std::string_view> stringHash;

	ShaderUniformData::UniformKey ShaderUniformData::MakeKey(const char* name)
	{
		std::string_view view(name);
		return stringHash(view);
	}

	ShaderUniformData::UniformKey ShaderUniformData::MakeKey(const string& name)
	{
		return stringHash(name);
	}

	void ShaderUniformData::Clear()
	{
		Version = TemporaryVersion;
		Uniforms.clear();
		Textures.clear();
	}

	void ShaderUniformData::MergeWith(const ShaderUniformData& other)
	{
		for (const auto& kvp : other.Uniforms)
			Uniforms[kvp.first] = kvp.second;

		for (const auto& kvp : other.Textures)
			Textures[kvp.first] = kvp.second;
	}
}