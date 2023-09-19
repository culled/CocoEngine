#pragma once

#include "../Renderpch.h"
#include "RenderPassBinding.h"
#include "CompiledRenderPipeline.h"

namespace Coco::Rendering
{
	/// @brief Defines a set of RenderPasses for rendering
	class RenderPipeline
	{
	private:
		std::vector<RenderPassBinding> _renderPasses;
		CompiledRenderPipeline _compiledPipeline;
		bool _isDirty;

	public:
		RenderPipeline();
		~RenderPipeline();

		/// @brief Adds a RenderPass to this pipeline
		/// @param renderPass The pass
		/// @param bindingIndices Indices that correspond to this pipeline's list of attachments
		/// @return True if the pass was added
		bool AddRenderPass(SharedRef<RenderPass> renderPass, std::span<uint8> bindingIndices);

		/// @brief Removes a RenderPass from this pipeline
		/// @param renderPass The pass
		void RemoveRenderPass(const SharedRef<RenderPass>& renderPass);

		/// @brief Compiles this pipeline if it is dirty
		/// @return True if compilation was successful
		bool Compile();

		/// @brief Gets the compiled pipeline. Make sure to Compile() before using this
		/// @return The compiled pipeline
		const CompiledRenderPipeline& GetCompiledPipeline() const { return _compiledPipeline; }

	private:
		/// @brief Marks this pipeline as dirty and needing to be re-compiled
		void MarkDirty();
	};
}