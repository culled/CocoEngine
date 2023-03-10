#include "Material.h"

namespace Coco::Rendering
{
	Material::Material(Ref<Rendering::Shader> shader) :
		Shader(shader)
	{
	}

	Material::~Material()
	{
		Shader.reset();
	}

	Ref<MaterialInstance> Material::CreateInstance() const
	{
		return CreateRef<MaterialInstance>(this);
	}

	Color Material::GetColor(const string& name) const noexcept
	{
		Vector4 value = GetVector4(name);
		return Color(value.X, value.Y, value.Z, value.W);
	}

	MaterialInstance::MaterialInstance(const Material* material) : Material(material->Shader)
	{
	}

	MaterialInstance::~MaterialInstance()
	{
	}
}
