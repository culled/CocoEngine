#pragma once

#include <Coco/Core/Types/String.h>
#include "Graphics/RenderPassShader.h"
#include "RendererResource.h"

namespace Coco::Rendering
{
	/// @brief Defines how geometry gets rendered
	class Shader : public RendererResource
	{
	private:
		string _groupTag;
		std::vector<RenderPassShader> _passShaders;

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

		/// @brief Adds a render pass shader to this shader
		/// @param passShader The pass shader
		void AddRenderPassShader(RenderPassShader&& passShader);

		/// @brief Gets this shader's render pass shaders
		/// @return A list of render pass shaders
		std::span<const RenderPassShader> GetRenderPassShaders() const { return _passShaders; }

		/// @brief Gets a render pass shader if this shader has one for the given pass
		/// @param renderPassName The name of the render pass
		/// @param outRenderPassShader Will be set to the RenderPassShader that corresponds to the given render pass name
		/// @return True if a render pass shader was found for the given render pass name
		bool TryGetRenderPassShader(const char* renderPassName, const RenderPassShader*& outRenderPassShader) const;
	};
}