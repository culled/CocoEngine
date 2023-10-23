#include "Renderpch.h"
#include "ShaderUniformData.h"

namespace Coco::Rendering
{
	const ShaderUniformData ShaderUniformData::Empty = ShaderUniformData();

	ShaderUniformData::ShaderUniformData() :
		ShaderUniformData(TemporaryVersion)
	{}

	ShaderUniformData::ShaderUniformData(uint64 version) :
		Version(version),
		Floats{},
		Float2s{},
		Float3s{},
		Float4s{},
		Mat4x4s{},
		Ints{},
		Int2s{},
		Int3s{},
		Int4s{},
		Bools{},
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

	ShaderUniformData::Mat4x4 ShaderUniformData::ToMat4x4(const Matrix4x4& v) { return v.AsFloatArray(); }

	void ShaderUniformData::Clear()
	{
		Version = TemporaryVersion;
		Floats.clear();
		Float2s.clear();
		Float3s.clear();
		Float4s.clear();
		Mat4x4s.clear();
		Ints.clear();
		Int2s.clear();
		Int3s.clear();
		Int4s.clear();
		Bools.clear();
		Textures.clear();
	}

	void ShaderUniformData::MergeWith(const ShaderUniformData& other)
	{
		for (const auto& kvp : other.Floats)
			Floats[kvp.first] = kvp.second;

		for (const auto& kvp : other.Float2s)
			Float2s[kvp.first] = kvp.second;

		for (const auto& kvp : other.Float3s)
			Float3s[kvp.first] = kvp.second;

		for (const auto& kvp : other.Float4s)
			Float4s[kvp.first] = kvp.second;

		for (const auto& kvp : other.Mat4x4s)
			Mat4x4s[kvp.first] = kvp.second;

		for (const auto& kvp : other.Ints)
			Ints[kvp.first] = kvp.second;

		for (const auto& kvp : other.Int2s)
			Int2s[kvp.first] = kvp.second;

		for (const auto& kvp : other.Int3s)
			Int3s[kvp.first] = kvp.second;

		for (const auto& kvp : other.Int4s)
			Int4s[kvp.first] = kvp.second;

		for (const auto& kvp : other.Bools)
			Bools[kvp.first] = kvp.second;

		for (const auto& kvp : other.Textures)
			Textures[kvp.first] = kvp.second;
	}
}