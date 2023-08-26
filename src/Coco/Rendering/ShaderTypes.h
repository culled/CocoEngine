#pragma once

#include <Coco/Core/API.h>

#include <Coco/Core/Types/String.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/List.h>
#include "Graphics/Resources/BufferTypes.h"
#include "Graphics/GraphicsPipelineState.h"
#include <Coco/Core/Resources/ResourceTypes.h>

namespace Coco::Rendering
{
	/// @brief Types of shader stages
	enum class ShaderStageType
	{
		Vertex,
		Tesselation,
		Geometry,
		Fragment,
		Compute,
		Unknown
	};

	/// @brief Types of shader descriptors
	enum class ShaderDescriptorType
	{
		UniformColor,
		UniformVector4
	};

	/// @brief An attribute that represents a kind of data within a contiguous vertex buffer
	struct COCOAPI ShaderVertexAttribute
	{
		friend struct Subshader;

	private:
		/// @brief The auto-calculated offset from the start of the vertex data structure of this attribute
		uint32_t _dataOffset;

	public:
		/// @brief The format of the data for this attribute
		BufferDataFormat DataFormat;

		ShaderVertexAttribute(BufferDataFormat dataFormat) noexcept;

		/// @brief Gets the auto-calculated offset from the start of the vertex data structure of this attribute
		/// @return The offset from the start of the vertex data structure of this attribute
		constexpr uint32_t GetDataOffset() const noexcept { return _dataOffset; }
	};

	/// @brief A descriptor for a shader
	struct COCOAPI ShaderDescriptor
	{
		/// @brief The descriptor name (used for referencing from materials)
		string Name;

		/// @brief The type of descriptor
		BufferDataFormat Type;

		ShaderDescriptor(const string& name, BufferDataFormat type) noexcept;
	};

	/// @brief A texture sampler for a shader
	struct COCOAPI ShaderTextureSampler
	{
		/// @brief The descriptor name (used for referencing from materials)
		string Name;

		ShaderTextureSampler(const string& name) noexcept;
	};

	/// @brief A container for shader uniform data
	struct ShaderUniformData
	{
		/// @brief The ID of this data
		ResourceID ID;

		/// @brief The version of this data
		ResourceVersion Version;

		/// @brief If true, this data will be preserved in the renderer between frames
		bool Preserve;

		/// @brief The vector4 properties
		UnorderedMap<string, Vector4> Vector4s;

		/// @brief The color properties
		UnorderedMap<string, Color> Colors;

		/// @brief The texture properties
		UnorderedMap<string, ResourceID> Textures;

		ShaderUniformData();

		/// @brief Merges another shader uniform data object with this one. By default, only uniform data that the other object has will be merged into this one
		/// @param other The object to merge into this one
		/// @param overwriteProperties If true, any shared uniforms will be overwritten by the other's uniforms
		void Merge(const ShaderUniformData& other, bool overwriteProperties = false);

		/// @brief Copies properties from another shader uniform data object. Only copies shared uniforms between the two objects
		/// @param other The object to copy from
		void CopyFrom(const ShaderUniformData& other);
	};

	/// @brief A stage within a shader
	struct COCOAPI ShaderStage
	{
		/// @brief The name of the stage's entry point
		string EntryPointName;

		/// @brief The type of this stage
		ShaderStageType Type;

		/// @brief The file for the stage
		string FilePath;

		ShaderStage();
		ShaderStage(const string& entryPointName, ShaderStageType stageType, const string& filePath);
		virtual ~ShaderStage() = default;
	};

	/// @brief A render-pass specific shader
	struct Subshader
	{
		friend class Shader;

		/// @brief The name of the render pass that uses this subshader
		string PassName;

		/// @brief Stages within this subshader
		List<ShaderStage> Stages;

		/// @brief The graphics pipeline state that this subshader requires
		GraphicsPipelineState PipelineState;

		/// @brief Vertex shader attributes for this subshader
		List<ShaderVertexAttribute> Attributes;

		/// @brief Descriptors for this subshader
		List<ShaderDescriptor> Descriptors;

		/// @brief Texture samplers for this subshader
		List<ShaderTextureSampler> Samplers;

		/// @brief The point in the render pipeline when the descriptors should be bound
		ShaderStageType DescriptorBindingPoint = ShaderStageType::Fragment;

		Subshader() = default;

		Subshader(
			const string& name,
			const List<ShaderStage>& stages,
			const GraphicsPipelineState& pipelineState,
			const List<ShaderVertexAttribute>& attributes,
			const List<ShaderDescriptor>& descriptors,
			const List<ShaderTextureSampler>& samplers,
			ShaderStageType bindPoint = ShaderStageType::Fragment) noexcept;

		/// @brief Converts uniform data into a single list of data that can be loaded into a buffer
		/// @param data The uniform data
		/// @param minimumAlignment The minimum alignment for the data
		/// @return The uniform data as bytes
		List<char> GetUniformData(const ShaderUniformData& data, uint minimumAlignment) const;

		/// @brief Gets the size of this subshader's descriptors
		/// @param minimumAlignment The minimum alignment for the descriptors
		/// @return The number of bytes required for the descriptor data
		uint64_t GetDescriptorDataSize(uint minimumAlignment) const;

	private:
		/// @brief Updates this subshader's vertex attribute offsets
		void UpdateAttributeOffsets() noexcept;
	};
}