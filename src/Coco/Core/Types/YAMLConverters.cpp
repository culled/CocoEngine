#include "Corepch.h"
#include "YAMLConverters.h"
#include "../Resources/Resource.h"
#include "../Engine.h"

namespace YAML
{
    Emitter& operator<<(Emitter& out, const Coco::FilePath& v)
    {
        out << v.ToString();

        return out;
    }

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

    Emitter& operator<<(Emitter& out, const Coco::Matrix4x4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq;
        
        for (int i = 0; i < Coco::Matrix4x4::CellCount; i++)
        {
            out << v.Data.at(i);
        }

        out << YAML::EndSeq;

        return out;
    }

    Emitter& operator<<(Emitter& out, const Coco::Resource& res)
    {
        using namespace Coco;
        FilePath resPath = Engine::Get()->GetAssetManager().GetResourceFilePath(res.GetID());

        if(resPath.IsEmpty())
        {
            out << FormatString("~{}", res.GetID().ToString());
        }
        else
        {
            out << resPath.ToString();
        }

        return out;
    }
}

namespace Coco
{
    SharedRef<Resource> LoadResourceFromYAML(const YAML::Node& baseNode)
    {
        string v = baseNode.as<string>();

        if (v[0] == '~')
        {
            ResourceID id(v.substr(1));
            return Engine::Get()->GetResourceLibrary().Get(id);
        }
        else
        {
            return Engine::Get()->GetAssetManager().GetOrLoad(v);
        }
    }
}