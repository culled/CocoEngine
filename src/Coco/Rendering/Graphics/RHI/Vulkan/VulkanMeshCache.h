#pragma once
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Resources/ResourceTypes.h>
#include "../../../MeshTypes.h"

#include "VulkanBuffer.h"

namespace Coco::Rendering
{
    class Mesh;
}

namespace Coco::Rendering::Vulkan
{
    class VulkanGraphicsDevice;

    struct CachedVulkanMesh
    {
        WeakSharedRef<Mesh> MeshRef;
        ResourceID MeshID;
        ResourceVersion MeshVersion;

        Ref<VulkanBuffer> Buffer;
        std::optional<VulkanBufferAllocationBlock> AllocBlock;

        VertexDataFormat VertexFormat;
        uint64 VertexCount;

        uint64 IndexBufferOffset;
        uint64 IndexCount;

        double LastUsageTime;

        CachedVulkanMesh(const SharedRef<Mesh>& mesh);
        CachedVulkanMesh(const CachedVulkanMesh& otherMesh);

        void Update(const Mesh& mesh);
        void Update(const CachedVulkanMesh& otherMesh);

        void Use();
    };

    class VulkanMeshCache
    {
    public:
        VulkanMeshCache(VulkanGraphicsDevice& device, bool isDynamic);
        ~VulkanMeshCache();

        const CachedVulkanMesh& CreateOrUpdateMesh(const SharedRef<Mesh>& mesh);
        bool HasMesh(const ResourceID& meshID) const;
        const CachedVulkanMesh& GetMesh(const ResourceID& meshID);
        bool TryGetMesh(const ResourceID& meshID, const CachedVulkanMesh*& outCachedMesh);
        void RemoveMesh(const ResourceID& meshID);
        void Clear();
        uint64 PurgeUnusedMeshDatas();

        void CopyMesh(const CachedVulkanMesh& sourceMesh);

    private:
        VulkanGraphicsDevice& _device;
        bool _isDynamic;
        std::unordered_map<ResourceID, CachedVulkanMesh> _cachedMeshes;
        Ref<VulkanBuffer> _meshBuffer;

    private:
        void EnsureCachedDataIsAllocated(uint64 totalDataSize, CachedVulkanMesh& cachedMesh);
        void FreeCachedMeshData(CachedVulkanMesh& cachedMesh);
    };
}