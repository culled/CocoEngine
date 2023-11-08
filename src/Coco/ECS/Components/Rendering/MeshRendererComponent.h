#pragma once
#include <Coco/Rendering/Providers/MaterialDataProvider.h>
#include <Coco/Rendering/Mesh.h>

using namespace Coco::Rendering;

namespace Coco::ECS
{
    /// @brief A component that stores information for rendering a mesh
    struct MeshRendererComponent
    {
        /// @brief The visibility groups of this mesh renderer
        uint64 VisibilityGroups;

        /// @brief The mesh to render
        SharedRef<Mesh> Mesh;

        /// @brief The materials to render each submesh with
        std::unordered_map<uint32, SharedRef<MaterialDataProvider>> Materials;

        MeshRendererComponent();
        MeshRendererComponent(SharedRef<Rendering::Mesh> mesh, const std::unordered_map<uint32, SharedRef<MaterialDataProvider>>& materials);
        MeshRendererComponent(SharedRef<Rendering::Mesh> mesh, const std::unordered_map<uint32, SharedRef<MaterialDataProvider>>& materials, uint64 visibilityGroups);

        /// @brief Populates the material slots based on the assigned mesh's submeshes
        void EnsureMaterialSlots();
    };
}