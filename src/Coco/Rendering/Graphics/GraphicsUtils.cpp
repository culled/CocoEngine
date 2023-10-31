#include "Renderpch.h"
#include "GraphicsUtils.h"

namespace Coco::Rendering
{
	shaderc_shader_kind ShaderStageToShaderC(ShaderStageType stage)
	{
		switch (stage)
		{
		case ShaderStageType::Vertex:
			return shaderc_vertex_shader;
		case ShaderStageType::Tesselation:
			return shaderc_tess_control_shader;
		case ShaderStageType::Geometry:
			return shaderc_geometry_shader;
		case ShaderStageType::Fragment:
			return shaderc_fragment_shader;
		case ShaderStageType::Compute:
			return shaderc_compute_shader;
		default:
			return shaderc_vertex_shader;
		}
	}

	BufferDataType SPIRTypeToBufferDataType(const spirv_cross::SPIRType& type)
	{
		switch (type.basetype)
		{
		case spirv_cross::SPIRType::Boolean:
			return BufferDataType::Bool;
		case spirv_cross::SPIRType::Float:
		{
			switch (type.vecsize)
			{
			case 1:
				return BufferDataType::Float;
			case 2:
				return BufferDataType::Float2;
			case 3:
				return BufferDataType::Float3;
			case 4:
			{
				switch (type.columns)
				{
				case 1:
					return BufferDataType::Float4;
				case 4:
					return BufferDataType::Mat4x4;
				default:
					break;
				}
			}
			default:
				break;
			}
		}
		case spirv_cross::SPIRType::Int:
		{
			switch (type.vecsize)
			{
			case 1:
				return BufferDataType::Int;
			case 2:
				return BufferDataType::Int2;
			case 3:
				return BufferDataType::Int3;
			case 4:
				return BufferDataType::Int4;
			default:
				break;
			}
		}
		default:
			break;
		}

		throw std::exception("Unsupported data type");
	}
}