#pragma once

#include "../EntityComponent.h"
#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/Mesh.h>

namespace Coco::ECS
{
    /// @brief A component that stores information for rendering a mesh
    class MeshRendererComponent :
        public EntityComponent
    {
        friend class MeshRendererComponentSerializer;

    public:
        MeshRendererComponent(const Entity& owner);
        MeshRendererComponent(
            const Entity& owner, 
            SharedRef<Rendering::Mesh> mesh, 
            const std::unordered_map<uint32, SharedRef<Rendering::Material>>& materials, 
            uint64 visibilityGroups = 1);

        // Inherited via EntityComponent
        const char* GetComponentTypename() const override { return "MeshRendererComponent"; }

        /// @brief Sets the mesh that will be rendered
        /// @param mesh The mesh
        void SetMesh(SharedRef<Rendering::Mesh> mesh);

        /// @brief Gets the rendered mesh
        /// @return The mesh
        SharedRef<Rendering::Mesh> GetMesh() const { return _mesh; }

        /// @brief Sets the material of a material slot
        /// @param slotIndex The index of the slot
        /// @param material The material
        void SetMaterial(uint32 slotIndex, SharedRef<Rendering::Material> material);

        /// @brief Sets the materials
        /// @param materials The slots and their corresponding materials
        void SetMaterials(const std::unordered_map<uint32, SharedRef<Rendering::Material>>& materials);

        /// @brief Gets the materials used for rendering and their corresponding submesh slots
        /// @return The map of submesh slots and materials
        const std::unordered_map<uint32, SharedRef<Rendering::Material>>& GetMaterials() const { return _materials; }
        std::unordered_map<uint32, SharedRef<Rendering::Material>>& GetMaterials() { return _materials; }

        /// @brief Sets the visibility groups of this renderer
        /// @param visibilityGroups The visibility groups
        void SetVisibilityGroups(uint64 visibilityGroups);

        /// @brief Gets the visibility groups of this renderer
        /// @return The visibility groups
        uint64 GetVisibilityGroups() const { return _visibilityGroups; }

    private:
        uint64 _visibilityGroups;
        SharedRef<Rendering::Mesh> _mesh;
        std::unordered_map<uint32, SharedRef<Rendering::Material>> _materials;

    private:
        /// @brief Populates/trims material slots based on the assigned mesh's submeshes
        void EnsureMaterialSlots();
    };
}