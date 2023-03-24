#include "Material.h"

#include "RenderingUtilities.h"
#include "RenderingService.h"
#include "Graphics/GraphicsDevice.h"

namespace Coco::Rendering
{
	Material::Material(Ref<Rendering::Shader> shader, const string& name) : RenderingResource(name, ResourceType::Material),
		Shader(shader)
	{
		UpdatePropertyMaps(true);
	}

	Material::~Material()
	{
		Shader.reset();
	}

	//Ref<MaterialInstance> Material::CreateInstance() const
	//{
	//	return CreateRef<MaterialInstance>(this);
	//}

	void Material::SetVector4(const string& name, const Vector4& value)
	{
		UpdatePropertyMaps(false);

		auto it = Vector4Properties.find(name);

		if (it != Vector4Properties.end())
		{
			(*it).second = value;
			this->IncrementVersion();
			_isBufferDataDirty = true;
		}
		else
		{
			LogError(GetRenderingLogger(), FormattedString("Shader \"{}\" has no vector4 property named \"{}\"", Shader->Name, name));
		}
	}

	Vector4 Material::GetVector4(const string& name) const
	{
		const auto it = Vector4Properties.find(name);

		if (it != Vector4Properties.end())
		{
			return (*it).second;
		}
		else
		{
			LogError(GetRenderingLogger(), FormattedString("Shader \"{}\" has no vector4 property named \"{}\"", Shader->Name, name));
			return Vector4::Zero;
		}
	}

	void Material::SetTexture(const string& name, Ref<Texture> texture)
	{
		UpdatePropertyMaps(false);

		auto it = TextureProperties.find(name);

		if (it != TextureProperties.end())
		{
			(*it).second = texture;
			this->IncrementVersion();
		}
		else
		{
			LogError(GetRenderingLogger(), FormattedString("Shader \"{}\" has no texture property named \"{}\"", Shader->Name, name));
		}
	}

	Ref<Texture> Material::GetTexture(const string& name) const
	{
		const auto it = TextureProperties.find(name);

		if (it != TextureProperties.cend())
		{
			return (*it).second;
		}
		else
		{
			LogError(GetRenderingLogger(), FormattedString("Shader \"{}\" has no texture property named \"{}\"", Shader->Name, name));
			return nullptr;
		}
	}

	const List<uint8_t>& Material::GetBufferData()
	{
		if (_isBufferDataDirty)
			UpdateBufferData();

		return _bufferData;
	}

	bool Material::TryGetSubshaderBinding(const string& subshaderName, const SubshaderUniformBinding*& binding)
	{
		if (_isBufferDataDirty)
			UpdateBufferData();

		const auto it = _subshaderBindings.find(subshaderName);

		if (it != _subshaderBindings.end())
		{
			binding = &(*it).second;
			return true;
		}

		return false;
	}

	void Material::UpdatePropertyMaps(bool forceUpdate)
	{
		if (!forceUpdate && _propertyMapVersion == Shader->GetVersion())
			return;

		Map<string, Vector4> vec4Properties;
		Map<string, Ref<Texture>> textureProperties;

		List<Subshader> subshaders = Shader->GetSubshaders();

		for (const Subshader& subshader : subshaders)
		{
			for (int i = 0; i < subshader.Descriptors.Count(); i++)
			{
				const ShaderDescriptor& descriptor = subshader.Descriptors[i];

				switch (descriptor.Type)
				{
				case BufferDataFormat::Vector4:
				{
					// Skip duplicate properties
					if (vec4Properties.contains(descriptor.Name))
						continue;

					if (Vector4Properties.contains(descriptor.Name))
						vec4Properties[descriptor.Name] = Vector4Properties[descriptor.Name];
					else
						vec4Properties[descriptor.Name] = Vector4::Zero;
					break;
				}
				default:
					break;
				}
			}

			for (int i = 0; i < subshader.Samplers.Count(); i++)
			{
				const ShaderTextureSampler& sampler = subshader.Samplers[i];

				// Skip duplicate properties
				if (textureProperties.contains(sampler.Name))
					continue;

				if (TextureProperties.contains(sampler.Name))
					textureProperties[sampler.Name] = TextureProperties[sampler.Name];
				else
					textureProperties[sampler.Name] = nullptr;
				break;
			}
		}

		Vector4Properties = std::move(vec4Properties);
		TextureProperties = std::move(textureProperties);
		_propertyMapVersion = Shader->GetVersion();
		_isBufferDataDirty = true;
	}

	void Material::UpdateBufferData()
	{
		RenderingService* renderService = EnsureRenderingService();

		const uint alignment = renderService->GetPlatform()->GetDevice()->GetMinimumBufferAlignment();
		const uint64_t alignedVec4Size = RenderingUtilities::GetOffsetForAlignment(Vector4Size, alignment);
		Array<float, 4> tempVec4 = { 0.0f };

		List<Subshader> subshaders = Shader->GetSubshaders();
	
		_subshaderBindings.clear();
		_bufferData.Clear();
		uint64_t offset = 0;

		for (const Subshader& subshader : subshaders)
		{
			Map<string, SubshaderUniformBinding>::iterator bindingIt = _subshaderBindings.end();
			if (subshader.Descriptors.Count() > 0)
				bindingIt = _subshaderBindings.emplace(subshader.PassName, SubshaderUniformBinding(offset)).first;

			for (int i = 0; i < subshader.Descriptors.Count(); i++)
			{
				const ShaderDescriptor& descriptor = subshader.Descriptors[i];

				switch (descriptor.Type)
				{
				case BufferDataFormat::Vector4:
				{
					Vector4 vec4 = GetVector4(descriptor.Name);

					tempVec4[0] = static_cast<float>(vec4.X);
					tempVec4[1] = static_cast<float>(vec4.Y);
					tempVec4[2] = static_cast<float>(vec4.Z);
					tempVec4[3] = static_cast<float>(vec4.W);

					_bufferData.Resize(_bufferData.Count() + alignedVec4Size);

					uint8_t* dst = (_bufferData.Data() + offset);
					std::memcpy(dst, tempVec4.data(), tempVec4.size() * sizeof(float));

					offset += alignedVec4Size;
					break;
				}
				default:
					break;
				}
			}

			if (bindingIt != _subshaderBindings.end())
			{
				SubshaderUniformBinding& binding = (*bindingIt).second;
				binding.Size = offset - binding.Offset;
			}
		}

		_isBufferDataDirty = false;
	}

	//MaterialInstance::MaterialInstance(const Material* material) : Material(material->Shader, FormattedString("{} (Instance)", material->Name))
	//{}
	//
	//MaterialInstance::~MaterialInstance()
	//{}
}
