#pragma once

#include <Coco/Core/Resources/Resource.h>

#include "Shader.h"
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Map.h>
#include "Texture.h"

namespace Coco::Rendering
{
	class MaterialInstance;

	class COCOAPI Material : public Resource
	{
		friend MaterialInstance;

	protected:
		Ref<Shader> Shader;
		Map<string, Vector4> Vector4Parameters;
		Map<string, Ref<Texture>> TextureParameters;
		Map<string, List<uint8_t>> StructParameters;

	public:
		Material(Ref<Rendering::Shader> shader);
		virtual ~Material();

		Ref<MaterialInstance> CreateInstance() const;

		Ref<Rendering::Shader> GetShader() const { return Shader; }

		void SetColor(const string& name, const Color& value) { SetVector4(name, Vector4(value.R, value.G, value.B, value.A)); }
		void ClearColor(const string& name) { ClearVector4(name); }
		Color GetColor(const string& name) const;

		void SetVector4(const string& name, const Vector4& value) { Vector4Parameters[name] = value; }
		void ClearVector4(const string& name) { Vector4Parameters.erase(name); }
		Vector4 GetVector4(const string& name) const { return GetFromMap(name, Vector4Parameters); }

		void SetTexture(const string& name, Ref<Texture> texture) { TextureParameters[name] = texture; }
		void ClearTexture(const string& name) { TextureParameters.erase(name); }
		Ref<Texture> GetTexture(const string& name) const { return GetFromMap(name, TextureParameters); }

		template<typename StructT>
		void SetStruct(const string& name, const StructT& data)
		{
			List<uint8_t> rawData(sizeof(data));
			std::memcpy(rawData.Data(), &data, rawData.Count());
			StructParameters[name] = rawData;
		}
		void ClearStruct(const string& name) { StructParameters.erase(name); }
		List<uint8_t> GetStructData(const string& name) { return StructParameters[name]; }

	private:
		template<typename ObjectT>
		ObjectT GetFromMap(const string& name, const Map<string, ObjectT>& map) const
		{
			auto it = map.find(name);

			if (it == map.end())
				return ObjectT();

			return (*it).second;
		}
	};

	class COCOAPI MaterialInstance : public Material
	{
	public:
		MaterialInstance(const Material* material);
		virtual ~MaterialInstance() override;
	};
}