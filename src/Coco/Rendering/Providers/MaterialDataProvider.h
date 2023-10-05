#pragma once

#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/Refs.h>

namespace Coco::Rendering
{
	class Shader;
	struct ShaderUniformData;

	/// @brief Base class for all providers of material data
	class MaterialDataProvider
	{
	public:
		virtual ~MaterialDataProvider() = default;

		/// @brief Gets the ID of this material instance
		/// @return The instance ID
		virtual uint64 GetMaterialID() const = 0;

		/// @brief Gets the uniform data of this material instance
		/// @return The uniform data
		virtual ShaderUniformData GetUniformData() const = 0;

		/// @brief Gets the shader used by this material instance
		/// @return The shader
		virtual SharedRef<Shader> GetShader() const = 0;
	};
}