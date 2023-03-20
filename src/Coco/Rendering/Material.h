#pragma once

#include "RenderingResource.h"

#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Map.h>
#include "Shader.h"
#include "Texture.h"
#include "Graphics/Resources/BufferTypes.h"

namespace Coco::Rendering
{
	class MaterialInstance;

	/// <summary>
	/// A binding for a subshader to a material
	/// </summary>
	struct SubshaderUniformBinding
	{
		uint64_t Offset;
		uint64_t Size = 0;

		SubshaderUniformBinding(uint64_t offset) :
			Offset(offset), Size(0) {}
	};

	/// <summary>
	/// A material that can be used to modify the look of rendered geometry
	/// </summary>
	class COCOAPI Material : public RenderingResource
	{
		friend MaterialInstance;

	public:
		static constexpr uint Vector4Size = GetBufferDataFormatSize(BufferDataFormat::Vector4);

	protected:
		ResourceVersion PropertyMapVersion = 0;
		Ref<Shader> Shader;
		Map<string, Vector4> Vector4Parameters;
		Map<string, Ref<Texture>> TextureParameters;

	private:
		Map<string, SubshaderUniformBinding> _subshaderBindings;
		List<uint8_t> _bufferData;
		bool _isBufferDataDirty = true;

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
		/// Sets a vector4 property
		/// </summary>
		/// <param name="name">The name of the vector4 property</param>
		/// <param name="value">The vector4</param>
		void SetVector4(const string& name, const Vector4& value);

		/// <summary>
		/// Gets a vector4 property
		/// </summary>
		/// <param name="name">The name of the vector4 property</param>
		/// <returns>The vector4, or a default vector4 if no property was found</returns>
		Vector4 GetVector4(const string& name) const;

		/// <summary>
		/// Sets a texture property
		/// </summary>
		/// <param name="name">The name of the texture property</param>
		/// <param name="texture">The texture</param>
		void SetTexture(const string& name, Ref<Texture> texture);

		/// <summary>
		/// Gets a texture property
		/// </summary>
		/// <param name="name">The name of the texture property</param>
		/// <returns>The texture, or a null reference if no property was found</returns>
		Ref<Texture> GetTexture(const string& name) const;

		/// <summary>
		/// Gets uniform buffer data from this material's currently set properties
		/// </summary>
		/// <returns>This material's properties as a byte array</returns>
		const List<uint8_t>& GetBufferData();

		bool TryGetSubshaderBinding(const string& subshaderName, SubshaderUniformBinding*& binding);

	private:
		/// <summary>
		/// Updates the property maps from this material's shader
		/// </summary>
		/// <param name="forceUpdate">If true, the map will be updated regardless if it matches the version of the shader</param>
		void UpdatePropertyMaps(bool forceUpdate);

		/// <summary>
		/// Updates this material's buffer data from its currently set properties
		/// </summary>
		void UpdateBufferData();
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