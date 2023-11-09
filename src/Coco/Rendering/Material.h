#pragma once

#include "RendererResource.h"
#include <Coco/Core/Types/Color.h>
#include "Providers/MaterialDataProvider.h"
#include "Graphics/ShaderUniformData.h"
#include "Graphics/ShaderUniformTypes.h"
#include "Texture.h"
#include "Shader.h"

namespace Coco::Rendering
{
	/// @brief A parameter for a material
	struct MaterialParameter
	{
		/// @brief The name of the parameter
		string Name;

		/// @brief The type of the parameter
		ShaderUniformType Type;

		/// @brief The parameter value
		std::any Value;

		MaterialParameter();
		MaterialParameter(const string& name, ShaderUniformType type, std::any value);
		MaterialParameter(const string& name, float value);
		MaterialParameter(const string& name, const Vector2& value);
		MaterialParameter(const string& name, const Vector3& value);
		MaterialParameter(const string& name, const Vector4& value);
		MaterialParameter(const string& name, const Color& value);
		MaterialParameter(const string& name, const Matrix4x4& value);
		MaterialParameter(const string& name, int value);
		MaterialParameter(const string& name, const Vector2Int& value);
		MaterialParameter(const string& name, const Vector3Int& value);
		MaterialParameter(const string& name, const Vector4Int& value);
		MaterialParameter(const string& name, bool value);
		MaterialParameter(const string& name, const SharedRef<Texture>& value);

		virtual ~MaterialParameter() = default;

		/// @brief Gets this parameter's value
		/// @tparam ValueType The type of value
		/// @return The value
		template<typename ValueType>
		ValueType As() const;

		/// @brief Gets this parameter's value as a float
		/// @return The value
		template<>
		float As<float>() const;

		/// @brief Gets this parameter's value as a Vector2
		/// @return The value
		template<>
		Vector2 As<Vector2>() const;

		/// @brief Gets this parameter's value as a Vector3
		/// @return The value
		template<>
		Vector3 As<Vector3>() const;

		/// @brief Gets this parameter's value as a Vector4
		/// @return The value
		template<>
		Vector4 As<Vector4>() const;

		/// @brief Gets this parameter's value as a Color
		/// @return The value
		template<>
		Color As<Color>() const;

		/// @brief Gets this parameter's value as a Matrix4x4
		/// @return The value
		template<>
		Matrix4x4 As<Matrix4x4>() const;

		/// @brief Gets this parameter's value as a int
		/// @return The value
		template<>
		int As<int>() const;

		/// @brief Gets this parameter's value as a Vector2Int
		/// @return The value
		template<>
		Vector2Int As<Vector2Int>() const;

		/// @brief Gets this parameter's value as a Vector3Int
		/// @return The value
		template<>
		Vector3Int As<Vector3Int>() const;

		/// @brief Gets this parameter's value as a Vector4Int
		/// @return The value
		template<>
		Vector4Int As<Vector4Int>() const;

		/// @brief Gets this parameter's value as a bool
		/// @return The value
		template<>
		bool As<bool>() const;

		/// @brief Gets this parameter's value as a texture resource
		/// @return The value
		template<>
		SharedRef<Texture> As<SharedRef<Texture>>() const;

		/// @brief Gets this parameter's value as a ShaderUniformUnion
		/// @return The value
		template<>
		ShaderUniformUnion As<ShaderUniformUnion>() const;
	};

	/// @brief Describes how to render a surface
	class Material : 
		public RendererResource, 
		public MaterialDataProvider
	{
		friend class MaterialSerializer;

	private:
		std::unordered_map<string, MaterialParameter> _parameters;

	public:
		Material(const ResourceID& id, const string& name);

		std::type_index GetType() const final { return typeid(Material); }

		uint64 GetMaterialID() const final { return GetID(); }
		ShaderUniformData GetUniformData() const final;

		/// @brief Sets the value of a parameter
		/// @tparam ValueType The type of value to set
		/// @param name The name of the parameter
		/// @param value The value
		template<typename ValueType>
		void SetValue(const char* name, const ValueType& value)
		{
			// TODO: only allow parameter creation through explicit means?
			_parameters[name] = MaterialParameter(name, value);

			IncrementVersion();
		}

		/// @brief Gets the value of a parameter
		/// @tparam ValueType The type of value to get. NOTE: this must match the type of the parameter
		/// @param name The name of the parameter
		/// @return The parameter's value
		template<typename ValueType>
		ValueType GetValue(const char* name) const
		{
			auto it = _parameters.find(name);

			if (it == _parameters.end())
				return ValueType();

			return it->second.As<ValueType>();
		}

		/// @brief Determines if this material has a parameter with the given name
		/// @param name The parameter name
		/// @return True if a parameter with the given name exists
		bool HasParameter(const char* name) const;

		/// @brief Performs a callback function for each parameter in this material
		/// @param callback The callback function
		void ForEachParameter(std::function<void(const MaterialParameter&)> callback) const;

		/// @brief Adds parameters to this material from a given shader
		/// @param shader The shader
		void AddParametersFromShader(const Shader& shader);

	private:
		/// @brief Increments the version of this material
		void IncrementVersion();
	};
}