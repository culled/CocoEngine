#pragma once

#include <Coco/Rendering/RenderingResource.h>

#include <Coco/Core/Types/List.h>
#include "BufferTypes.h"

namespace Coco::Rendering
{
	/// @brief A buffer that holds contiguous data for a graphics device
	class COCOAPI Buffer : public RenderingResource
	{
	public:
		/// @brief The usage flags of this buffer
		const BufferUsageFlags UsageFlags;

	public:
		Buffer(ResourceID id, const string& name, BufferUsageFlags usageFlags) noexcept;
		virtual ~Buffer() = default;

		Buffer() = delete;
		Buffer(const Buffer&) = delete;
		Buffer(Buffer&&) = delete;

		Buffer& operator=(const Buffer&) = delete;
		Buffer& operator=(Buffer&&) = delete;

		/// @brief Resizes this buffer to a new size, optionally retaining its data (as long as it isn't culled by shrinking).
		/// NOTE: if the data will only be copied if this buffer was created with the BufferUsageFlags::TransferDestination and BufferUsageFlags::TransferSource flags
		/// @param newSize The new size (in bytes)
		/// @param copyOldData If true, the buffer data will be copied over to the resized one. Set to false for a slight performance benefit
		virtual void Resize(uint64_t newSize, bool copyOldData) = 0;

		/// @brief Copies a part of this buffer to part of another buffer
		/// @param sourceOffset The offset to start copying from (in bytes)
		/// @param destination The destination buffer
		/// @param destinationOffset The offset of the destination to start copying to (in bytes)
		/// @param size The number of bytes to copy
		virtual void CopyTo(uint64_t sourceOffset, Buffer* destination, uint64_t destinationOffset, uint64_t size) = 0;

		/// @brief Binds this buffer to graphics memory
		/// @param offset The offset to start the bound memory from (in bytes)
		virtual void Bind(uint64_t offset) = 0;

		/// @brief Gets the size of this buffer
		/// @return This buffer's size (in bytes)
		virtual uint64_t GetSize() const noexcept = 0;

		/// @brief Makes this buffer accessible for copying to/from the host.
		/// NOTE: This buffer must have been created with the BufferUsageFlags::HostVisible flag for this to work
		/// @param offset The offset in the buffer of the first byte to lock
		/// @param size The number of bytes to lock<
		/// @return A pointer to the first locked byte of buffer memory
		virtual void* Lock(uint64_t offset, uint64_t size) = 0;

		/// @brief Unlocks this buffer
		virtual void Unlock() = 0;

		/// @brief Loads contiguous data into this buffer
		/// @tparam DataType 
		/// @param offset The offset in the buffer to start loading data to (in bytes)
		/// @param data The list of data to load
		template<typename DataType>
		void LoadData(uint64_t offset, const List<DataType>& data)
		{
			uint64_t byteSize = sizeof(DataType) * data.Count();
			void* bufferData = Lock(offset, byteSize);
			std::memcpy(bufferData, reinterpret_cast<const void*>(data.Data()), byteSize);
			Unlock();
		}

		/// @brief Loads data into this buffer
		/// @tparam DataType 
		/// @param offset The offset in the buffer to start loading data to (in bytes)
		/// @param size The number of bytes to load in
		/// @param data The data to load
		template<typename DataType>
		void LoadData(uint64_t offset, uint64_t size, const DataType* data)
		{
			void* bufferData = Lock(offset, size);
			std::memcpy(bufferData, reinterpret_cast<const void*>(data), size);
			Unlock();
		}
	};
}