#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Resources/Resource.h>
#include "RenderPipelineTypes.h"
#include "RenderPassBinding.h"
#include "CompiledRenderPipeline.h"

namespace Coco::Rendering
{
	/// @brief Defines a set of RenderPasses for rendering
	class RenderPipeline : 
		public Resource
	{
	private:
		AttachmentOptionFlags _defaultAttachmentOptions;
		std::unordered_map<uint8, AttachmentOptionFlags> _attachmentOptions;
		std::vector<RenderPassBinding> _renderPasses;
		CompiledRenderPipeline _compiledPipeline;
		bool _isDirty;

	public:
		RenderPipeline(const ResourceID& id, const string& name, AttachmentOptionFlags defaultAttachmentOptions = AttachmentOptionFlags::None);
		~RenderPipeline();

		std::type_index GetType() const final { return typeid(RenderPipeline); }

		/// @brief Adds a RenderPass to this pipeline
		/// @param renderPass The pass
		/// @param bindingIndices Indices that correspond to this pipeline's list of attachments
		/// @return True if the pass was added
		bool AddRenderPass(SharedRef<RenderPass> renderPass, std::span<const uint8> bindingIndices);

		/// @brief Removes a RenderPass from this pipeline
		/// @param renderPass The pass
		void RemoveRenderPass(const SharedRef<RenderPass>& renderPass);

		/// @brief Gets the number of render passes in this pipeline
		/// @return The number of render passes
		uint64 GetRenderPassCount() const { return _renderPasses.size(); }

		/// @brief Gets the render passes in this pipeline
		/// @return The render passes in this pipeline
		std::span<RenderPassBinding> GetRenderPasses() { return _renderPasses; }

		/// @brief Sets the default options for all attachments
		/// @param options The attachment options
		void SetDefaultAttachmentOptions(AttachmentOptionFlags options);

		/// @brief Gets the default options for attachments
		/// @return The default attachment options
		AttachmentOptionFlags GetDefaultAttachmentOptions() const { return _defaultAttachmentOptions; }

		/// @brief Sets the options for a pipeline attachment
		/// @param pipelineAttachmentIndex The index of the pipeline attachment
		/// @param options The options for the attachment
		void SetAttachmentOptions(uint8 pipelineAttachmentIndex, AttachmentOptionFlags options);

		/// @brief Removes the options for a pipeline attachment. This will revert it to the default attachment options for this pipeline
		/// @param pipelineAttachmentIndex The index of the pipeline attachment
		void RemoveAttachmentOptions(uint8 pipelineAttachmentIndex);

		/// @brief Gets the options for a pipeline attachment
		/// @param pipelineAttachmentIndex The index of the pipeline attachment
		/// @return The options for the attachment
		AttachmentOptionFlags GetAttachmentOptions(uint8 pipelineAttachmentIndex) const;

		/// @brief Compiles this pipeline if it is dirty
		/// @return True if compilation was successful
		bool Compile();

		/// @brief Gets the compiled pipeline. Make sure to Compile() before using this
		/// @return The compiled pipeline
		const CompiledRenderPipeline& GetCompiledPipeline() const { return _compiledPipeline; }

	private:
		/// @brief Marks this pipeline as dirty and needing to be re-compiled
		void MarkDirty();

		/// @brief Checks if any renderpasses differ in state from their compiled versions
		void CheckIfRenderPassesDirty();
	};
}