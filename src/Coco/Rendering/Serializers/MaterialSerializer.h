#pragma once
#include <Coco\Core\Resources\ResourceSerializer.h>

namespace Coco::Rendering
{
    class Material;
    class MaterialInstance;

    /// @brief A serializer for Material and MaterialInstance resources
    class MaterialSerializer :
        public ResourceSerializer
    {
        // Inherited via ResourceSerializer
        bool SupportsFileExtension(const string& extension) const final;
        bool SupportsResourceType(const std::type_index& type) const final;
        const std::type_index GetResourceTypeForExtension(const string& extension) const final;
        string Serialize(Ref<Resource> resource) final;
        ManagedRef<Resource> Deserialize(const std::type_index& resourceType, const ResourceID& resourceID, const string& data) final;

    private:
        /// @brief Serializes a Material
        /// @param material The material
        /// @return The serialized data
        string SerializeMaterial(const Material& material);

        /// @brief Deserializes a Material
        /// @param resourceID The ID for the resource
        /// @param data The serialized data
        /// @return The deserialized resource
        ManagedRef<Resource> DeserializeMaterial(const ResourceID& resourceID, const string& data);

        /// @brief Serializes a MaterialInstance
        /// @param material The material
        /// @return The serialized data
        string SerializeMaterialInstance(const MaterialInstance& material);

        /// @brief Deserializes a MaterialInstance
        /// @param resourceID The ID for the resource
        /// @param data The serialized data
        /// @return The deserialized resource
        ManagedRef<Resource> DeserializeMaterialInstance(const ResourceID& resourceID, const string& data);
    };
}