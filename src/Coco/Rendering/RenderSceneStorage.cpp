//
// Created by cullen on 3/21/26.
//

#include "RenderSceneStorage.h"

namespace Coco
{
    ShaderUniformGroup::ShaderUniformGroup(Span<const ShaderUniformValue> uniforms) :
        Uniforms(uniforms)
    {}

    RenderSceneStorage::RenderSceneStorage(Allocator* allocator, uint64 rawDataPageSize, uint64 uniformPageSize) :
        _allocator(allocator),
        _pageSize(rawDataPageSize),
        _allocators(allocator, 2),
        _dataMap(allocator),
        _shaderUniformValues(uniformPageSize, allocator),
        _shaderUniformGroups(allocator)
    {}

    RenderSceneStorage::~RenderSceneStorage()
    {
        _shaderUniformGroups.Clear();
        _shaderUniformValues.Clear();

        _dataMap.Clear();

        for (auto& allocator : _allocators)
            allocator.Reset();

        _allocators.Clear(true);
    }

    void RenderSceneStorage::StoreUniforms(uint64 id, Span<const ShaderUniformValue> uniforms)
    {
        if (_shaderUniformGroups.Contains(id))
            return;

        auto data = _shaderUniformValues.Allocate(uniforms);
        _shaderUniformGroups.Emplace(id, data);
    }

    bool RenderSceneStorage::HasUniforms(uint64 id) const
    {
        return _shaderUniformGroups.Contains(id);
    }

    Span<const ShaderUniformValue> RenderSceneStorage::GetUniforms(uint64 id) const
    {
        return _shaderUniformGroups.Get(id).Uniforms;
    }

    void RenderSceneStorage::Clear()
    {
        _shaderUniformGroups.Clear();
        _shaderUniformValues.Clear();

        _dataMap.Clear();

        for (auto& allocator : _allocators)
            allocator.Reset();
    }
} // Coco