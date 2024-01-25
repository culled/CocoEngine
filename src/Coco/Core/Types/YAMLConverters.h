#pragma once

#include "../IO/FilePath.h"
#include "Refs.h"
#include "Vector.h"
#include "Quaternion.h"
#include "Color.h"
#include "Matrix.h"
#include "Matrix.h"
#include <yaml-cpp/yaml.h>

namespace Coco
{
	class Resource;
}

namespace YAML
{
	Emitter& operator<<(Emitter& out, const Coco::FilePath& v);

	template<>
	struct convert<Coco::FilePath>
	{
		static Node encode(const Coco::FilePath& v)
		{
			Node node;
			node.push_back(v.ToString());
			return node;
		}

		static bool decode(const Node& node, Coco::FilePath& v)
		{
			if (!node.IsScalar())
				return false;

			v = Coco::FilePath(node.as<Coco::string>());

			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const Coco::Vector2& v);

	template<>
	struct convert<Coco::Vector2>
	{
		static Node encode(const Coco::Vector2& v)
		{
			Node node;
			node.push_back(v.X);
			node.push_back(v.Y);
			return node;
		}

		static bool decode(const Node& node, Coco::Vector2& v)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			v.X = node[0].as<double>();
			v.Y = node[1].as<double>();

			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const Coco::Vector2Int& v);

	template<>
	struct convert<Coco::Vector2Int>
	{
		static Node encode(const Coco::Vector2Int& v)
		{
			Node node;
			node.push_back(v.X);
			node.push_back(v.Y);
			return node;
		}

		static bool decode(const Node& node, Coco::Vector2Int& v)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			v.X = node[0].as<int>();
			v.Y = node[1].as<int>();

			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const Coco::Vector3& v);

	template<>
	struct convert<Coco::Vector3>
	{
		static Node encode(const Coco::Vector3& v)
		{
			Node node;
			node.push_back(v.X);
			node.push_back(v.Y);
			node.push_back(v.Z);
			return node;
		}

		static bool decode(const Node& node, Coco::Vector3& v)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			v.X = node[0].as<double>();
			v.Y = node[1].as<double>();
			v.Z = node[2].as<double>();

			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const Coco::Vector3Int& v);

	template<>
	struct convert<Coco::Vector3Int>
	{
		static Node encode(const Coco::Vector3Int& v)
		{
			Node node;
			node.push_back(v.X);
			node.push_back(v.Y);
			node.push_back(v.Z);
			return node;
		}

		static bool decode(const Node& node, Coco::Vector3Int& v)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			v.X = node[0].as<int>();
			v.Y = node[1].as<int>();
			v.Z = node[2].as<int>();

			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const Coco::Vector4& v);

	template<>
	struct convert<Coco::Vector4>
	{
		static Node encode(const Coco::Vector4& v)
		{
			Node node;
			node.push_back(v.X);
			node.push_back(v.Y);
			node.push_back(v.Z);
			node.push_back(v.W);
			return node;
		}

		static bool decode(const Node& node, Coco::Vector4& v)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			v.X = node[0].as<double>();
			v.Y = node[1].as<double>();
			v.Z = node[2].as<double>();
			v.W = node[3].as<double>();

			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const Coco::Vector4Int& v);

	template<>
	struct convert<Coco::Vector4Int>
	{
		static Node encode(const Coco::Vector4Int& v)
		{
			Node node;
			node.push_back(v.X);
			node.push_back(v.Y);
			node.push_back(v.Z);
			node.push_back(v.W);
			return node;
		}

		static bool decode(const Node& node, Coco::Vector4Int& v)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			v.X = node[0].as<int>();
			v.Y = node[1].as<int>();
			v.Z = node[2].as<int>();
			v.W = node[3].as<int>();

			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const Coco::Quaternion& v);

	template<>
	struct convert<Coco::Quaternion>
	{
		static Node encode(const Coco::Quaternion& v)
		{
			Node node;
			node.push_back(v.W);
			node.push_back(v.X);
			node.push_back(v.Y);
			node.push_back(v.Z);
			return node;
		}

		static bool decode(const Node& node, Coco::Quaternion& v)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			v.W = node[0].as<double>();
			v.X = node[1].as<double>();
			v.Y = node[2].as<double>();
			v.Z = node[3].as<double>();

			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const Coco::Color& v);

	template<>
	struct convert<Coco::Color>
	{
		static Node encode(const Coco::Color& v)
		{
			Node node;
			node.push_back(v.R);
			node.push_back(v.G);
			node.push_back(v.B);
			node.push_back(v.A);
			node.push_back(v.IsLinear);
			return node;
		}

		static bool decode(const Node& node, Coco::Color& v)
		{
			if (!node.IsSequence() || node.size() != 5)
				return false;

			v.R = node[0].as<double>();
			v.G = node[1].as<double>();
			v.B = node[2].as<double>();
			v.A = node[3].as<double>();
			v.IsLinear = node[4].as<bool>();

			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const Coco::Matrix4x4& v);

	template<>
	struct convert<Coco::Matrix4x4>
	{
		static Node encode(const Coco::Matrix4x4& v)
		{
			Node node;

			for (int i = 0; i < Coco::Matrix4x4::CellCount; i++)
			{
				node.push_back(v.Data.at(i));
			}

			return node;
		}

		static bool decode(const Node& node, Coco::Matrix4x4& v)
		{
			if (!node.IsSequence() || node.size() != Coco::Matrix4x4::CellCount)
				return false;

			for (int i = 0; i < Coco::Matrix4x4::CellCount; i++)
			{
				v.Data.at(i) = node.as<double>();
			}

			return true;
		}
	};

	template<typename ItemType>
	Emitter& operator<<(Emitter& out, std::span<const ItemType> valueArray)
	{
		for (const auto& value : valueArray)
			out << value;

		return out;
	}

	Emitter& operator<<(Emitter& out, const Coco::Resource& res);
}

namespace Coco
{
	SharedRef<Resource> LoadResourceFromYAML(const YAML::Node& baseNode);
}