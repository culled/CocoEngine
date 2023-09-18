#include "Renderpch.h"
#include "ShaderUniformData.h"

namespace Coco::Rendering
{
	const ShaderUniformData ShaderUniformData::Empty = ShaderUniformData();

	ShaderUniformData::ShaderUniformData() :
		Version(TemporaryVersion),
		Floats{},
		Float2s{},
		Float3s{},
		Float4s{},
		Mat4x4s{},
		Ints{},
		Int2s{},
		Int3s{},
		Int4s{},
		Bools{}
	{}

	std::hash<std::string_view> stringHash;

	ShaderUniformData::UniformKey ShaderUniformData::MakeKey(const char* name)
	{
		std::string_view view(name);
		return stringHash(view);
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
	}
}