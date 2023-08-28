#pragma once

#include <Coco/Core/API.h>
#include "ShaderTypes.h"

namespace Coco::Rendering
{
	/// @brief An interface for material data
	class COCOAPI IMaterial
	{
	public:
		/// @brief Gets this material's ID
		/// @return This material's ID
		virtual const ResourceID& GetID() const = 0;

		/// @brief Gets this material's version
		/// @return This material's version
		virtual ResourceVersion GetMaterialVersion() const = 0;

		/// @brief Sets uniform data for this material
		/// @param uniformData The shader uniform data
		virtual void SetUniformData(const ShaderUniformData& uniformData) = 0;

		/// @brief Gets uniform data from this material
		/// @return The shader uniform data
		virtual ShaderUniformData GetUniformData() const = 0;

		/// @brief Gets the shader that this material uses
		/// @return The shader that this material uses
		virtual Ref<Shader> GetShader() const noexcept = 0;
	};
}