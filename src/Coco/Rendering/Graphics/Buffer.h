#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Defines.h>
#include "BufferTypes.h"
#include <Coco/Core/Types/Freelist.h>

namespace Coco::Rendering
{
	class Buffer
	{
	public:
		virtual ~Buffer() = default;

		/// @brief Gets this buffer's usage flags
		/// @return This buffer's usage flags
		virtual BufferUsageFlags GetUsageFlags() const = 0;

		/// @brief Gets the size of this buffer
		/// @return This buffer's size, in bytes
		virtual uint64 GetSize() const = 0;

		/// @brief Gets the unallocated space in this buffer
		/// @return The unallocated space
		virtual uint64 GetFreeSpace() const = 0;

		/// @brief Resizes this buffer to a new size, optionally retaining any data that doesn't become truncated.
		/// NOTE: the data will only be copied if this buffer was created with the BufferUsageFlags::TransferDestination and BufferUsageFlags::TransferSource flags
		/// @param newSize The new size in bytes
		/// @param copyOldData If true, the buffer data will be copied over to the resized one. Set to false for a slight performance benefit
		virtual void Resize(uint64 newSize, bool copyOldData) = 0;

		/// @brief Copies a part of this buffer to part of another buffer
		/// @param sourceOffset The offset to start copying from, in bytes
		/// @param destination The destination buffer
		/// @param destinationOffset The offset of the destination to start copying to, in bytes
		/// @param size The number of bytes to copy
		virtual void CopyTo(uint64 sourceOffset, Buffer& destination, uint64 destinationOffset, uint64 size) = 0;

		/// @brief Finds a region in this buffer that fits the required size
		/// @param requiredSize The required size of the region, in bytes
		/// @param outBlock Will be set to the block of allocated memory if successful
		/// @return True if a region was found
		virtual bool Allocate(uint64 requiredSize, FreelistNode& outBlock) = 0;

		/// @brief Frees a region in this buffer
		/// @param block The block to free
		virtual void Free(const FreelistNode& block) = 0;

		/// @brief Frees all allocated regions in this buffer
		virtual void FreeAllAllocations() = 0;

		/// @brief Makes this buffer accessible for copying to/from the host.
		/// NOTE: This buffer must have been created with the BufferUsageFlags::HostVisible flag for this to work
		/// @param offset The offset of the first byte in the buffer to lock
		/// @param size The number of bytes to lock
		/// @return A pointer to the first locked byte of buffer memory
		virtual void* Lock(uint64 offset, uint64 size) = 0;

		/// @brief Unlocks this buffer
		virtual void Unlock() = 0;

		/// @brief Loads data into this buffer
		/// @tparam DataType The type of data to load
		/// @param offset The offset in the buffer to start loading data to, in bytes
		/// @param data The data to load
		template<typename DataType>
		void LoadData(uint64 offset, std::span<DataType> data)
		{
			uint64 byteSize = sizeof(DataType) * data.size();
			void* bufferData = Lock(offset, byteSize);
			std::memcpy(bufferData, reinterpret_cast<const void*>(data.data()), byteSize);
			Unlock();
		}

		/// @brief Loads data into this buffer
		/// @tparam DataType The type of data to load
		/// @param offset The offset in the buffer to start loading data to, in bytes
		/// @param data The data to load
		/// @param size The number of bytes to load in
		template<typename DataType>
		void LoadData(uint64 offset, const DataType* data, uint64 size)
		{
			void* bufferData = Lock(offset, size);
			std::memcpy(bufferData, reinterpret_cast<const void*>(data), size);
			Unlock();
		}
	};
}