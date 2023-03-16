#pragma once

#include <Coco/Core/Resources/Resource.h>

#include "Shader.h"
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Map.h>
#include "Texture.h"

namespace Coco::Rendering
{
	class MaterialInstance;

	/// <summary>
	/// A material that can be used to modify the look of rendered geometry
	/// </summary>
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
		virtual ~Material() override;

		/// <summary>
		/// Creates an instance of this material that allows for its properties to differ from this material
		/// </summary>
		/// <returns>An instance of this material</returns>
		Ref<MaterialInstance> CreateInstance() const;

		/// <summary>
		/// Gets the shader that this material uses
		/// </summary>
		/// <returns>The shader that this material uses</returns>
		Ref<Rendering::Shader> GetShader() const noexcept { return Shader; }

		/// <summary>
		/// Sets a color property as a vector4 property
		/// </summary>
		/// <param name="name">The name of the color property</param>
		/// <param name="value">The color</param>
		void SetColor(const string& name, const Color& value) { SetVector4(name, Vector4(value.R, value.G, value.B, value.A)); }

		/// <summary>
		/// Clears a color property
		/// </summary>
		/// <param name="name">The name of the color property</param>
		void ClearColor(const string& name) noexcept { ClearVector4(name); }

		/// <summary>
		/// Gets a color property
		/// </summary>
		/// <param name="name">The name of the color property</param>
		/// <returns>The color, or a default color if no property was found</returns>
		Color GetColor(const string& name) const noexcept;

		/// <summary>
		/// Sets a vector4 property
		/// </summary>
		/// <param name="name">The name of the vector4 property</param>
		/// <param name="value">The vector4</param>
		void SetVector4(const string& name, const Vector4& value) { Vector4Parameters[name] = value; }

		/// <summary>
		/// Clears a vector4 property
		/// </summary>
		/// <param name="name">The name of the vector4 property</param>
		void ClearVector4(const string& name) noexcept;

		/// <summary>
		/// Gets a vector4 property
		/// </summary>
		/// <param name="name">The name of the vector4 property</param>
		/// <returns>The vector4, or a default vector4 if no property was found</returns>
		Vector4 GetVector4(const string& name) const noexcept { return GetFromMap(name, Vector4Parameters); }

		/// <summary>
		/// Sets a texture property
		/// </summary>
		/// <param name="name">The name of the texture property</param>
		/// <param name="texture">The texture</param>
		void SetTexture(const string& name, Ref<Texture> texture) { TextureParameters[name] = texture; }

		/// <summary>
		/// Clears a texture property
		/// </summary>
		/// <param name="name">The name of the texture property</param>
		void ClearTexture(const string& name) noexcept;

		/// <summary>
		/// Gets a texture property
		/// </summary>
		/// <param name="name">The name of the texture property</param>
		/// <returns>The texture, or a null reference if no property was found</returns>
		Ref<Texture> GetTexture(const string& name) const noexcept { return GetFromMap(name, TextureParameters); }

		/// <summary>
		/// Sets a struct property
		/// </summary>
		/// <param name="name">The name of the struct property</param>
		/// <param name="data">The struct data</param>
		template<typename StructT>
		void SetStruct(const string& name, const StructT& data)
		{
			List<uint8_t> rawData(sizeof(data));
			std::memcpy(rawData.Data(), &data, rawData.Count());
			StructParameters[name] = rawData;
		}

		/// <summary>
		/// Clears a struct property
		/// </summary>
		/// <param name="name">The name of the struct property</param>
		void ClearStruct(const string& name) { StructParameters.erase(name); }

		/// <summary>
		/// Gets a struct property's data
		/// </summary>
		/// <param name="name">The name of the struct property</param>
		/// <returns>The byte data of the struct, or an empty list if no property was found</returns>
		List<uint8_t> GetStructData(const string& name) { return StructParameters[name]; }

	private:
		/// <summary>
		/// Attempts to find an element via name in a map
		/// </summary>
		/// <param name="name">The name of the element to find</param>
		/// <param name="map">The map to search in</param>
		/// <returns>The found element, or a default object if none was found</returns>
		template<typename ObjectT>
		ObjectT GetFromMap(const string& name, const Map<string, ObjectT>& map) const noexcept
		{
			try
			{
				auto it = map.find(name);

				if (it == map.end())
					return ObjectT();

				return (*it).second;
			}
			catch (...)
			{
				return ObjectT();
			}
		}
	};

	// TODO: make this actually reference the parent material and override its properties

	/// <summary>
	/// An instance of a material that overrides properties from the parent material
	/// </summary>
	class COCOAPI MaterialInstance : public Material
	{
	public:
		MaterialInstance(const Material* material);
		virtual ~MaterialInstance() override;
	};
}