#pragma once

#include "RendererResource.h"
#include <Coco/Core/Types/String.h>
#include "Graphics/ShaderVariant.h"

namespace Coco::Rendering
{
	/// @brief Defines how geometry gets rendered
	class Shader : public RendererResource
	{
		friend class ShaderSerializer;

	private:
		string _groupTag;
		std::vector<ShaderVariant> _variants;

	public:
		Shader(const ResourceID& id, const string& name, const string& groupTag);
		~Shader();

		std::type_index GetType() const final { return typeid(Shader); }

		/// @brief Sets this shader's group tag
		/// @param groupTag The group tag
		void SetGroupTag(const char* groupTag);

		/// @brief Gets this shader's group tag
		/// @return The group tag
		const string& GetGroupTag() const { return _groupTag; }

		/// @brief Adds a shader variant to this shader
		/// @param variant The shader variant
		void AddVariant(ShaderVariant&& variant);

		/// @brief Gets this shader's variants
		/// @return A list of shader variants
		std::span<const ShaderVariant> GetShaderVariants() const { return _variants; }

		/// @brief Tries to get a variant of this shader by name
		/// @param variantName The name of the variant
		/// @param outShaderVariant Will be set to the ShaderVariant that corresponds to the given name
		/// @return True if a shader variant was found for the given name
		bool TryGetShaderVariant(const char* variantName, const ShaderVariant*& outShaderVariant) const;
	};
}