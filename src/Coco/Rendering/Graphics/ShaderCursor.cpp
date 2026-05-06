//
// Created by cullen on 3/21/26.
//

#include "ShaderCursor.h"
#include "Resources/ShaderProgram.h"
#include "Coco/Core/Engine.h"
#include "Coco/Rendering/Texture.h"
#include <slang.h>

namespace Coco
{
    ShaderElementLocation::ShaderElementLocation() :
        ShaderElementLocation(0, 0, 0)
    {}

    ShaderElementLocation::ShaderElementLocation(uint64 byteOffset, uint32 bindingRangeOffset,
        uint32 arrayIndexInBindingRange) :
        ByteOffset(byteOffset),
        BindingRangeOffset(bindingRangeOffset),
        ArrayIndexInBindingRange(arrayIndexInBindingRange)
    {}

    ShaderCursor::ShaderCursor() :
        _typeLayout(nullptr),
        _currentLocation(),
        _bufferInterface(nullptr)
    {}

    ShaderCursor::ShaderCursor(ShaderBufferInterface* bufferInterface) :
        _typeLayout(bufferInterface->GetTypeLayout()),
        _currentLocation(),
        _bufferInterface(bufferInterface)
    {}

    ShaderCursor::ShaderCursor(slang::TypeLayoutReflection* typeLayout, const ShaderElementLocation& location, ShaderBufferInterface* bufferInterface) :
        _typeLayout(typeLayout),
        _currentLocation(location),
        _bufferInterface(bufferInterface)
    {}

    ShaderCursor::~ShaderCursor()
    {
        //if (_bufferInterface)
        //    _bufferInterface->Flush();
    }

    void ShaderCursor::BindToInterface(ShaderBufferInterface& bufferInterface)
    {
        _typeLayout = bufferInterface.GetTypeLayout();
        _currentLocation = ShaderElementLocation();
        _bufferInterface = &bufferInterface;
    }

    ShaderCursor ShaderCursor::Field(const char* name) const
    {
        int fieldIndex = static_cast<int>(_typeLayout->findFieldIndexByName(name));
        if (fieldIndex == -1)
        {
            COCO_ENGINE_LOG_ERROR("Invalid ShaderCursor field name \"%s\"", name);
            return ShaderCursor();
        }

        return Field(fieldIndex);
    }

    ShaderCursor ShaderCursor::Field(uint32 index) const
    {
        if (index >= _typeLayout->getFieldCount())
        {
            COCO_ENGINE_LOG_ERROR("Invalid ShaderCursor field index. 0 < %u < %u", index, _typeLayout->getFieldCount());
            return ShaderCursor();
        }

        auto field = _typeLayout->getFieldByIndex(index);

        ShaderElementLocation newLocation = _currentLocation;
        newLocation.ByteOffset += field->getOffset();
        newLocation.BindingRangeOffset += field->getBindingIndex();

        return ShaderCursor(field->getTypeLayout(), newLocation, _bufferInterface);
    }

    ShaderCursor ShaderCursor::ArrayElement(uint32 index) const
    {
        auto elementTypeLayout = _typeLayout->getElementTypeLayout();
        if (index >= _typeLayout->getElementCount())
        {
            COCO_ENGINE_LOG_ERROR("Invalid ShaderCursor array element. 0 < %u < %u", index, _typeLayout->getElementCount());
            return ShaderCursor();
        }

        ShaderElementLocation newLocation = _currentLocation;
        newLocation.ByteOffset += index * elementTypeLayout->getStride();

        // https://docs.shader-slang.org/en/latest/shader-cursors.html#elements
        newLocation.ArrayIndexInBindingRange *= _currentLocation.ArrayIndexInBindingRange * _typeLayout->getElementCount();
        newLocation.ArrayIndexInBindingRange += index;

        return ShaderCursor(elementTypeLayout, newLocation, _bufferInterface);
    }

    void ShaderCursor::Write(float value)
    {
        if (!_bufferInterface)
            return;

        _bufferInterface->Write(_currentLocation, &value, sizeof(float));
    }

    void ShaderCursor::Write(const Vector2& value)
    {
        if (!_bufferInterface)
            return;

        _bufferInterface->Write(_currentLocation, value.Raw, sizeof(Vector2));
    }

    void ShaderCursor::Write(const Vector3& value)
    {
        if (!_bufferInterface)
            return;

        _bufferInterface->Write(_currentLocation, value.Raw, sizeof(Vector3));
    }

    void ShaderCursor::Write(const Vector4& value)
    {
        if (!_bufferInterface)
            return;

        _bufferInterface->Write(_currentLocation, value.Raw, sizeof(Vector4));
    }

    void ShaderCursor::Write(int value)
    {
        if (!_bufferInterface)
            return;

        _bufferInterface->Write(_currentLocation, &value, sizeof(int));
    }

    void ShaderCursor::Write(const Vector2i& value)
    {
        if (!_bufferInterface)
            return;

        _bufferInterface->Write(_currentLocation, value.Raw, sizeof(Vector2i));
    }

    void ShaderCursor::Write(const Vector3i& value)
    {
        if (!_bufferInterface)
            return;

        _bufferInterface->Write(_currentLocation, value.Raw, sizeof(Vector3i));
    }

    void ShaderCursor::Write(const Vector4i& value)
    {
        if (!_bufferInterface)
            return;

        _bufferInterface->Write(_currentLocation, value.Raw, sizeof(Vector4i));
    }

    void ShaderCursor::Write(uint32 value)
    {
        if (!_bufferInterface)
            return;

        _bufferInterface->Write(_currentLocation, &value, sizeof(uint32));
    }

    void ShaderCursor::Write(Span<const uint32, 2> value)
    {
        if (!_bufferInterface)
            return;

        _bufferInterface->Write(_currentLocation, value.data(), sizeof(uint32) * value.size());
    }

    void ShaderCursor::Write(Span<const uint32, 3> value) {
        if (!_bufferInterface)
            return;

        _bufferInterface->Write(_currentLocation, value.data(), sizeof(uint32) * value.size());
    }

    void ShaderCursor::Write(Span<const uint32, 4> value) {
        if (!_bufferInterface)
            return;

        _bufferInterface->Write(_currentLocation, value.data(), sizeof(uint32) * value.size());
    }

    void ShaderCursor::Write(const Matrix4x4& value)
    {
        if (!_bufferInterface)
            return;

        _bufferInterface->Write(_currentLocation, value.Raw, sizeof(Matrix4x4));
    }

    void ShaderCursor::Write(const Color& value)
    {
        Write(value.AsVector4(false));
    }

    void ShaderCursor::Write(SharedPtr<Texture> texture)
    {
        if (!_bufferInterface)
            return;

        _bufferInterface->Write(_currentLocation, texture.get());
    }
} // Coco