#pragma once
#include <Coco\Core\Resources\ResourceSerializer.h>

namespace Coco::Rendering
{
    class Material;
    //class MaterialInstance;

    /// @brief A serializer for Material and MaterialInstance resources
    class MaterialSerializer :
        public ResourceSerializer
    {
        // Inherited via ResourceSerializer
        bool SupportsFileExtension(const string& extension) const final;
        bool SupportsResourceType(const std::type_index& type) const final;
        string Serialize(SharedRef<Resource> resource) final;
        SharedRef<Resource> CreateAndDeserialize(const ResourceID& id, const string& name, const string& data) final;
        bool Deserialize(const string& data, SharedRef<Resource> resource) final;

    private:
        /// @brief Serializes a Material
        /// @param material The material
        /// @return The serialized data
        string SerializeMaterial(const Material& material);

        /// @brief Deserializes a Material
        /// @param data The serialized data
        /// @param material The material to deserialize into
        /// @return True if the deserialization was successful
        bool DeserializeMaterial(const string& data, Material& material);

        /// @brief Serializes a MaterialInstance
        /// @param material The material
        /// @return The serialized data
        //string SerializeMaterialInstance(const MaterialInstance& material);

        /// @brief Deserializes a MaterialInstance
        /// @param resourceID The ID for the resource
        /// @param data The serialized data
        /// @return The deserialized resource
        //SharedRef<Resource> DeserializeMaterialInstance(const ResourceID& resourceID, const string& data);
    };
}