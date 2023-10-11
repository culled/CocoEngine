#include "Converters.h"

namespace YAML
{
    Emitter& operator<<(Emitter& out, const Coco::Vector2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.X << v.Y << YAML::EndSeq;

        return out;
    }

    Emitter& operator<<(Emitter& out, const Coco::Vector2Int& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.X << v.Y << YAML::EndSeq;

        return out;
    }

    Emitter& operator<<(Emitter& out, const Coco::Vector3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.X << v.Y << v.Z << YAML::EndSeq;

        return out;
    }

    Emitter& operator<<(Emitter& out, const Coco::Vector3Int& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.X << v.Y << v.Z << YAML::EndSeq;

        return out;
    }

    Emitter& operator<<(Emitter& out, const Coco::Vector4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.X << v.Y << v.Z << v.W << YAML::EndSeq;

        return out;
    }

    Emitter& operator<<(Emitter& out, const Coco::Vector4Int& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.X << v.Y << v.Z << v.W << YAML::EndSeq;

        return out;
    }

    Emitter& operator<<(Emitter& out, const Coco::Quaternion& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.W << v.X << v.Y << v.Z << YAML::EndSeq;

        return out;
    }

    Emitter& operator<<(Emitter& out, const Coco::Color& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.R << v.G << v.B << v.A << v.IsLinear << YAML::EndSeq;

        return out;
    }
}