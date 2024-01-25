#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Resources/Resource.h>
#include <Coco/Core/Types/CoreExceptions.h>
#include "RenderPass.h"
#include "RenderPassPipelineBinding.h"
#include "CompiledRenderPipeline.h"

namespace Coco::Rendering
{
	/// @brief An exception thrown when a RenderPipeline fails to compile
	class RenderPipelineCompileException :
		public Exception
	{
	public:
		RenderPipelineCompileException(const string& message);
	};

	/// @brief A pipeline that defines a set of RenderPasses to create a finished image
	class RenderPipeline :
		public Resource
	{
	public:
		RenderPipeline(const ResourceID& id, AttachmentOptionsFlags defaultAttachmentOptions = AttachmentOptionsFlags::None);

		// Inherited via Resource
		const std::type_info& GetType() const final { return typeid(RenderPipeline); }
		const char* GetTypename() const final { return "RenderPipeline"; }

		/// @brief Adds a RenderPass
		/// @param renderPass The RenderPass
		/// @param pipelineAttachmentBindingIndices Indicies of pipeline attachments that correspond to each of the RenderPass's attachments
		/// @return True if the RenderPass was added
		bool AddRenderPass(SharedRef<RenderPass> renderPass, std::span<const uint32> pipelineAttachmentBindingIndices);

		/// @brief Removes the first instance of a RenderPass from this pipeline
		/// @param renderPass The pass to remove
		void RemoveRenderPass(const SharedRef<RenderPass>& renderPass);

		/// @brief Gets the RenderPassPipelineBinding binding each RenderPass to this pipeline
		/// @return The bindings
		std::span<const RenderPassPipelineBinding> GetRenderPassBindings() { return _renderPasses; }

		/// @brief Gets/compiles this pipeline to a CompiledRenderPipeline
		/// @return The compiled representation of this pipeline
		const CompiledRenderPipeline& GetOrCompile();

	private:
		std::vector<RenderPassPipelineBinding> _renderPasses;
		std::optional<CompiledRenderPipeline> _compiledPipeline;
		AttachmentOptionsFlags _defaultAttachmentOptions;

	private:
		/// @brief Marks this pipeline as dirty
		void MarkDirty();
	};
}