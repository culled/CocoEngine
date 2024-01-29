#pragma once
#include <Coco/Core/Resources/Resource.h>
#include <Coco/Core/Types/Color.h>
#include "Graphics/ShaderUniformTypes.h"
#include "Texture.h"
#include "Shader.h"

namespace Coco::Rendering
{
    class Material :
        public Resource
    {
		friend class MaterialSerializer;

    public:
        Material(const ResourceID& id);

        // Inherited via Resource
		const std::type_info& GetType() const override { return typeid(Material); }
        const char* GetTypename() const override { return "Material"; }

		/// @brief Sets a value
		/// @tparam ValueType The type of value to set
		/// @param name The name of the value
		/// @param value The value
		template<typename ValueType>
		void SetValue(const string& name, const ValueType& value)
		{
			// TODO: only allow parameter creation through explicit means?
			auto it = FindValue(name);

			if (it != _parameters.end())
			{
				*it = ShaderUniformValue(name, value);
			}
			else
			{
				_parameters.emplace_back(name, value);
			}

			MarkDirty();
		}

		/// @brief Gets a value
		/// @tparam ValueType The type of value to get. NOTE: this must match the type of the value
		/// @param name The name of the value
		/// @return The value
		template<typename ValueType>
		ValueType GetValue(const string& name) const
		{
			auto it = FindValue(name);

			if (it == _parameters.end())
				return ValueType();

			return it->As<ValueType>();
		}

		/// @brief Determines if this material has a value with the given name
		/// @param name The value name
		/// @return True if a value with the given name exists
		bool HasValue(const string& name) const;

		ShaderUniformType GetValueType(const string& name) const;

		std::span<const ShaderUniformValue> GetUniformValues() const { return _parameters; }
		std::span<ShaderUniformValue> GetUniformValues() { return _parameters; }

		void MarkDirty();

	private:
		std::vector<ShaderUniformValue> _parameters;

		std::vector<ShaderUniformValue>::iterator FindValue(const string& name);
		std::vector<ShaderUniformValue>::const_iterator FindValue(const string& name) const;
    };
}