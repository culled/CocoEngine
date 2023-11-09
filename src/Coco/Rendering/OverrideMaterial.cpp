#include "Renderpch.h"
#include "OverrideMaterial.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	OverrideMaterialParameter::OverrideMaterialParameter() :
		OverrideMaterialParameter(MaterialParameter(), false)
	{}

	OverrideMaterialParameter::OverrideMaterialParameter(const MaterialParameter& original, bool active) :
		MaterialParameter(original),
		Active(active)
	{}

	OverrideMaterialParameter::OverrideMaterialParameter(const string& name, ShaderUniformType type, std::any value, bool active) :
		MaterialParameter(name, type, value),
		Active(active)
	{}

	OverrideMaterialParameter::OverrideMaterialParameter(const string& name, float value, bool active) :
		MaterialParameter(name, value),
		Active(active)
	{}

	OverrideMaterialParameter::OverrideMaterialParameter(const string& name, const Vector2 & value, bool active) :
		MaterialParameter(name, value),
		Active(active)
	{}

	OverrideMaterialParameter::OverrideMaterialParameter(const string& name, const Vector3& value, bool active) :
		MaterialParameter(name, value),
		Active(active)
	{}

	OverrideMaterialParameter::OverrideMaterialParameter(const string& name, const Vector4& value, bool active) :
		MaterialParameter(name, value),
		Active(active)
	{}

	OverrideMaterialParameter::OverrideMaterialParameter(const string& name, const Color& value, bool active) :
		MaterialParameter(name, value),
		Active(active)
	{}

	OverrideMaterialParameter::OverrideMaterialParameter(const string& name, const Matrix4x4& value, bool active) :
		MaterialParameter(name, value),
		Active(active)
	{}

	OverrideMaterialParameter::OverrideMaterialParameter(const string& name, int value, bool active) :
		MaterialParameter(name, value),
		Active(active)
	{}

	OverrideMaterialParameter::OverrideMaterialParameter(const string& name, const Vector2Int& value, bool active) :
		MaterialParameter(name, value),
		Active(active)
	{}

	OverrideMaterialParameter::OverrideMaterialParameter(const string& name, const Vector3Int& value, bool active) :
		MaterialParameter(name, value),
		Active(active)
	{}

	OverrideMaterialParameter::OverrideMaterialParameter(const string& name, const Vector4Int& value, bool active) :
		MaterialParameter(name, value),
		Active(active)
	{}

	OverrideMaterialParameter::OverrideMaterialParameter(const string& name, bool value, bool active) :
		MaterialParameter(name, value),
		Active(active)
	{}

	OverrideMaterialParameter::OverrideMaterialParameter(const string& name, const SharedRef<Texture>& value, bool active) :
		MaterialParameter(name, value),
		Active(active)
	{}

	OverrideMaterial::OverrideMaterial(const ResourceID& id, const string& name, SharedRef<Material> baseMaterial) :
		RendererResource(id, name),
		_baseMaterial(baseMaterial),
		_overrideParameters()
	{
		SyncParameters();
	}

	OverrideMaterial::~OverrideMaterial()
	{
		_baseMaterial.reset();
		_overrideParameters.clear();
	}

	ShaderUniformData OverrideMaterial::GetUniformData() const
	{
		ShaderUniformData data(GetVersion() + _baseMaterial->GetVersion());

		_baseMaterial->ForEachParameter(
			[overrideParams = _overrideParameters, &data](const MaterialParameter& param)
			{
				ShaderUniformData::UniformKey key = ShaderUniformData::MakeKey(param.Name);

				auto it = overrideParams.find(param.Name);

				const MaterialParameter* finalParam = nullptr;

				if (it != overrideParams.end() && it->second.Active)
				{
					finalParam = &(it->second);
				}
				else
				{
					finalParam = &param;
				}

				Assert(finalParam)

				if (finalParam->Type == ShaderUniformType::Texture)
				{
					SharedRef<Texture> tex = finalParam->As<SharedRef<Texture>>();

					if (tex)
						data.Textures[key] = ShaderUniformData::ToTextureSampler(tex->GetImage(), tex->GetImageSampler());
				}
				else
				{
					data.Uniforms[key] = finalParam->As<ShaderUniformUnion>();
				}
			}
		);

		return data;
	}

	void OverrideMaterial::SetOverrideActive(const char* name, bool overrideActive)
	{
		auto it = _overrideParameters.find(name);

		if (it == _overrideParameters.end())
			return;

		it->second.Active = overrideActive;

		IncrementVersion();
	}

	bool OverrideMaterial::HasParameter(const char* name) const
	{
		return _overrideParameters.contains(name) ? true : _baseMaterial->HasParameter(name);
	}

	void OverrideMaterial::ForEachParameter(std::function<void(const OverrideMaterialParameter&)> callback) const
	{
		for (const auto& kvp : _overrideParameters)
		{
			callback(kvp.second);
		}
	}

	void OverrideMaterial::IncrementVersion()
	{
		SetVersion(GetVersion() + 1);
	}

	void OverrideMaterial::SyncParameters()
	{
		auto oIt = _overrideParameters.begin();

		while (oIt != _overrideParameters.end())
		{
			if (!_baseMaterial->HasParameter(oIt->first.c_str()))
			{
				oIt = _overrideParameters.erase(oIt);
			}
			else
			{
				oIt++;
			}
		}

		_baseMaterial->ForEachParameter(
			[&](const MaterialParameter& param)
			{
				auto it = _overrideParameters.find(param.Name);

				if (it != _overrideParameters.end())
					return;

				_overrideParameters.try_emplace(param.Name, param, false);
			}
		);
	}
}