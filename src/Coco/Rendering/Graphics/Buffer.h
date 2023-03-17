#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>

#include "BufferTypes.h"
#include "GraphicsResource.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A buffer that holds contiguous data for a graphics device
	/// </summary>
	class COCOAPI Buffer : public IGraphicsResource
	{
	protected:
		/// <summary>
		/// The usage flags of this buffer
		/// </summary>
		BufferUsageFlags UsageFlags;

	protected:
		Buffer(BufferUsageFlags usageFlags) noexcept;

	public:
		virtual ~Buffer() = default;

		Buffer() = delete;
		Buffer(const Buffer&) = delete;
		Buffer(Buffer&&) = delete;

		Buffer& operator=(const Buffer&) = delete;
		Buffer& operator=(Buffer&&) = delete;

		/// <summary>
		/// Resizes this buffer to a new size, retaining its data (as long as it isn't culled by shrinking).
		/// NOTE: this creates a copy of the current buffer and copies data, 
		/// so this buffer must have been created with the BufferUsageFlags::TransferDestination and BufferUsageFlags::TransferSource flags
		/// </summary>
		/// <param name="newSize">The new size (in bytes)</param>
		virtual void Resize(uint64_t newSize) = 0;

		/// <summary>
		/// Copies a part of this buffer to part of another buffer
		/// </summary>
		/// <param name="sourceOffset">The offset to start copying from (in bytes)</param>
		/// <param name="destination">The destination buffer</param>
		/// <param name="destinationOffset">The offset of the destination to start copying to (in bytes)</param>
		/// <param name="size">The number of bytes to copy</param>
		virtual void CopyTo(uint64_t sourceOffset, Buffer* destination, uint64_t destinationOffset, uint64_t size) = 0;

		/// <summary>
		/// Binds this buffer to graphics memory
		/// </summary>
		/// <param name="offset">The offset to start the bound memory from (in bytes)</param>
		virtual void Bind(uint64_t offset) = 0;

		/// <summary>
		/// Gets the size of this buffer
		/// </summary>
		/// <returns>This buffer's size (in bytes)</returns>
		virtual uint64_t GetSize() const noexcept = 0;

		/// <summary>
		/// Makes this buffer accessible for copying to/from the host.
		/// This buffer must have been created with the BufferUsageFlags::HostVisible flag for this to work
		/// </summary>
		/// <param name="offset">The offset in the buffer of the first byte to lock</param>
		/// <param name="size">The number of bytes to lock</param>
		/// <returns>A pointer to the first locked byte of buffer memory</returns>
		virtual void* Lock(uint64_t offset, uint64_t size) = 0;

		/// <summary>
		/// Unlocks this buffer
		/// </summary>
		virtual void Unlock() = 0;

		/// <summary>
		/// Loads contiguous data into this buffer
		/// </summary>
		/// <typeparam name="T">The type of data to load</typeparam>
		/// <param name="offset">The offset in the buffer to start loading data to (in bytes)</param>
		/// <param name="data">The list of data to load</param>
		template<typename T>
		void LoadData(uint64_t offset, const List<T>& data)
		{
			uint64_t byteSize = sizeof(T) * data.Count();
			void* bufferData = Lock(offset, byteSize);
			std::memcpy(bufferData, reinterpret_cast<const void*>(data.Data()), byteSize);
			Unlock();
		}

		/// <summary>
		/// Loads an item into this buffer
		/// </summary>
		/// <typeparam name="T">The type of data to load</typeparam>
		/// <param name="offset">The offset in the buffer to start loading data to (in bytes)</param>
		/// <param name="size">The number of bytes to load in</param>
		/// <param name="data">The data to load</param>
		template<typename T>
		void LoadData(uint64_t offset, uint64_t size, const T* data)
		{
			void* bufferData = Lock(offset, size);
			std::memcpy(bufferData, reinterpret_cast<const void*>(data), size);
			Unlock();
		}
	};
}