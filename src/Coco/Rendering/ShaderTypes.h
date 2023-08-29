#pragma once

#include <Coco/Core/API.h>

#include <Coco/Core/Types/String.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Matrix.h>
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
		None = 0,
		Vertex = 1 << 0,
		Tesselation = 1 << 1,
		Geometry = 1 << 2,
		Fragment = 1 << 3,
		Compute = 1 << 4,
	};

	constexpr auto operator<=>(const ShaderStageType& a, const ShaderStageType& b)
	{
		return static_cast<int>(a) <=> static_cast<int>(b);
	}

	constexpr ShaderStageType operator |=(ShaderStageType& a, const ShaderStageType& b)
	{
		return static_cast<ShaderStageType>(static_cast<int>(a) | static_cast<int>(b));
	}

	constexpr ShaderStageType operator &=(ShaderStageType& a, const ShaderStageType& b)
	{
		return static_cast<ShaderStageType>(static_cast<int>(a) & static_cast<int>(b));
	}

	/// @brief Scopes of shader descriptors
	enum class ShaderDescriptorScope
	{
		/// @brief Updated once per frame
		Global,
		
		/// @brief Updated once per instance
		Instance,

		/// @brief Updated per draw call
		Draw
	};

	/// @brief An attribute that represents a kind of data within a contiguous vertex buffer
	struct COCOAPI ShaderVertexAttribute
	{
		friend struct Subshader;

	private:
		/// @brief The auto-calculated offset from the start of the vertex data structure of this attribute
		uint32_t _dataOffset;

	public:
		/// @brief The name of this attribute
		string Name;

		/// @brief The format of the data for this attribute
		BufferDataFormat DataFormat;

		ShaderVertexAttribute(const string& name, BufferDataFormat dataFormat) noexcept;

		/// @brief Gets the auto-calculated offset from the start of the vertex data structure of this attribute
		/// @return The offset from the start of the vertex data structure of this attribute
		constexpr uint32_t GetDataOffset() const noexcept { return _dataOffset; }
	};

	/// @brief A descriptor for a shader
	struct COCOAPI ShaderDescriptor
	{
		/// @brief The descriptor name (used for referencing from materials)
		string Name;

		/// @brief The scope of this descriptor
		ShaderDescriptorScope Scope;

		/// @brief The points in the rendering stage when this descriptor should be bound
		ShaderStageType BindingPoints;

		ShaderDescriptor(const string& name, ShaderDescriptorScope scope, ShaderStageType bindPoint) noexcept;
		virtual ~ShaderDescriptor() = default;
	};

	/// @brief A texture sampler for a shader
	struct COCOAPI ShaderTextureSampler : public ShaderDescriptor
	{
		ShaderTextureSampler(const string& name, ShaderDescriptorScope scope, ShaderStageType bindPoint) noexcept;
		virtual ~ShaderTextureSampler() = default;
	};

	/// @brief A uniform for a shader
	struct COCOAPI ShaderUniformDescriptor : public ShaderDescriptor
	{
		/// @brief The type of descriptor
		BufferDataFormat Type;

		ShaderUniformDescriptor(const string& name, ShaderDescriptorScope scope, ShaderStageType bindPoint, BufferDataFormat type) noexcept;
		virtual ~ShaderUniformDescriptor() = default;
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

		/// @brief The int properties
		UnorderedMap<string, int32_t> Ints;

		/// @brief The Vector2Int properties
		UnorderedMap<string, Vector2Int> Vector2Ints;

		/// @brief The Vector3Int properties
		UnorderedMap<string, Vector3Int> Vector3Ints;

		/// @brief The Vector4Int properties
		UnorderedMap<string, Vector4Int> Vector4Ints;

		/// @brief The float properties
		UnorderedMap<string, float> Floats;

		/// @brief The vector2 properties
		UnorderedMap<string, Vector2> Vector2s;

		/// @brief The vector3 properties
		UnorderedMap<string, Vector3> Vector3s;

		/// @brief The vector4 properties
		UnorderedMap<string, Vector4> Vector4s;

		/// @brief The color properties
		UnorderedMap<string, Color> Colors;

		/// @brief The matrix4x4 properties
		UnorderedMap<string, Matrix4x4> Matrix4x4s;

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

		/// @brief Uniforms for this subshader
		List<ShaderUniformDescriptor> Uniforms;

		/// @brief Texture samplers for this subshader
		List<ShaderTextureSampler> Samplers;

		Subshader() = default;

		Subshader(
			const string& name,
			const List<ShaderStage>& stages,
			const GraphicsPipelineState& pipelineState,
			const List<ShaderVertexAttribute>& attributes,
			const List<ShaderUniformDescriptor>& uniforms,
			const List<ShaderTextureSampler>& samplers) noexcept;

		/// @brief Gets a list of shader uniforms in the given scope
		/// @param scope The scope of the uniforms
		/// @return A list of uniforms in the given scope
		List<ShaderUniformDescriptor> GetScopedUniforms(ShaderDescriptorScope scope) const;

		/// @brief Gets a list of shader texture samplers in the given scope
		/// @param scope The scope of the texture samplers
		/// @return A list of texture samplers in the given scope
		List<ShaderTextureSampler> GetScopedSamplers(ShaderDescriptorScope scope) const;

		/// @brief Determines if this subshader has any descriptors or samplers in the given scope
		/// @param scope The scope
		/// @return True if this subshader has any descriptors or samplers in the given scope
		bool HasScope(ShaderDescriptorScope scope) const;

		/// @brief Determines the binding stages for all uniforms within the given scope
		/// @param scope The scope of the uniforms
		/// @return The binding stages for all uniforms within the given scope
		ShaderStageType GetUniformBindingStages(ShaderDescriptorScope scope) const;

		/// @brief Gets the size of one vertex's data from this subshader's attributes
		/// @return The size of one vertex's data
		uint32_t GetVertexDataSize() const;

	private:
		/// @brief Updates this subshader's vertex attribute offsets
		void UpdateAttributeOffsets() noexcept;
	};
}