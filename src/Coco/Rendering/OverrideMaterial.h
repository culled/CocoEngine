#pragma once

#include "RendererResource.h"
#include "Material.h"

namespace Coco::Rendering
{
	/// @brief A parameter for an OverrideMaterial
	struct OverrideMaterialParameter : 
		public MaterialParameter
	{
		/// @brief If true, this parameter will override the base material's parameter with the same name
		bool Active;

		OverrideMaterialParameter();
		OverrideMaterialParameter(const MaterialParameter& original, bool active);
		OverrideMaterialParameter(const string& name, ShaderUniformType type, std::any value, bool active);
		OverrideMaterialParameter(const string& name, float value, bool active);
		OverrideMaterialParameter(const string& name, const Vector2& value, bool active);
		OverrideMaterialParameter(const string& name, const Vector3& value, bool active);
		OverrideMaterialParameter(const string& name, const Vector4& value, bool active);
		OverrideMaterialParameter(const string& name, const Color& value, bool active);
		OverrideMaterialParameter(const string& name, const Matrix4x4& value, bool active);
		OverrideMaterialParameter(const string& name, int value, bool active);
		OverrideMaterialParameter(const string& name, const Vector2Int& value, bool active);
		OverrideMaterialParameter(const string& name, const Vector3Int& value, bool active);
		OverrideMaterialParameter(const string& name, const Vector4Int& value, bool active);
		OverrideMaterialParameter(const string& name, bool value, bool active);
		OverrideMaterialParameter(const string& name, const SharedRef<Texture>& value, bool active);
	};

	/// @brief A material derived from a base material that can override its parameters
	class OverrideMaterial : 
		public RendererResource,
		public MaterialDataProvider
	{
		friend class MaterialSerializer;

	private:
		SharedRef<Material> _baseMaterial;
		std::unordered_map<string, OverrideMaterialParameter> _overrideParameters;

	public:
		OverrideMaterial(const ResourceID& id, const string& name, SharedRef<Material> baseMaterial);
		~OverrideMaterial();

		std::type_index GetType() const final { return typeid(OverrideMaterial); }

		uint64 GetMaterialID() const final { return GetID(); }
		ShaderUniformData GetUniformData() const final;

		/// @brief Sets the value of a parameter
		/// @tparam ValueType The type of value to set
		/// @param name The name of the parameter
		/// @param value The value
		/// @param overrideActive If true, the parameter will override the base material's parameter
		template<typename ValueType>
		void SetValue(const char* name, const ValueType& value, bool overrideActive)
		{
			_overrideParameters[name] = OverrideMaterialParameter(name, value, overrideActive);

			IncrementVersion();
		}

		/// @brief Gets the value of a parameter
		/// @tparam ValueType The type of value to get. NOTE: this must match the type of the parameter
		/// @param name The name of the parameter
		/// @return The parameter's value, or the base material's parameter value if no override was specified
		template<typename ValueType>
		ValueType GetValue(const char* name) const
		{
			auto it = _overrideParameters.find(name);

			if (it == _overrideParameters.end())
				return _baseMaterial->GetValue<ValueType>(name);

			return it->second.As<ValueType>();
		}

		/// @brief Sets active state for a parameter override. NOTE: does nothing if the parameter doesn't exist
		/// @param name The name of the parameter
		/// @param overrideActive The override active state
		void SetOverrideActive(const char* name, bool overrideActive);

		/// @brief Determines if this material has a parameter with the given name
		/// @param name The parameter name
		/// @return True if a parameter with the given name exists
		bool HasParameter(const char* name) const;

		/// @brief Performs a callback function for each parameter in this material
		/// @param callback The callback function
		void ForEachParameter(std::function<void(const OverrideMaterialParameter&)> callback) const;

	private:
		/// @brief Increments the version of this material
		void IncrementVersion();

		void SyncParameters();
	};
}