#pragma once

#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/String.h>

namespace Coco::Rendering
{
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
	};
}