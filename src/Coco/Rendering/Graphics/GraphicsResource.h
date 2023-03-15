#pragma once

#include <Coco/Core/Core.h>
//#include "GraphicsDevice.h"

//#include <atomic>
#include <type_traits>

namespace Coco::Rendering
{
	class COCOAPI IGraphicsResource
	{
	protected:
		IGraphicsResource() = default;
	public:
		virtual ~IGraphicsResource() = default;

		IGraphicsResource(const IGraphicsResource&) = delete;
		IGraphicsResource(IGraphicsResource&&) = delete;

		IGraphicsResource& operator=(const IGraphicsResource&) = delete;
		IGraphicsResource& operator=(IGraphicsResource&&) = delete;
	};

	template<class ObjectT>
	using GraphicsResourceRef = std::shared_ptr<ObjectT>;

	/*template<class ObjectT>
	class COCOAPI GraphicsResourceRef
	{
		template<class ObjectV>
		friend class GraphicsResourceRef;

	private:
		ObjectT* _resource = nullptr;
		GraphicsDevice* _owningDevice = nullptr;
		Ref<std::atomic_uint64_t> _counter = nullptr;

	public:
		GraphicsResourceRef() = default;

		GraphicsResourceRef(ObjectT* resource, GraphicsDevice* device) :
			_resource(resource), _owningDevice(device)
		{
			_counter = CreateRef<std::atomic_uint64_t>(1);
		}

		//template<class ObjectV, std::enable_if<std::is_convertible<ObjectV*, ObjectT*>::value, bool> = true>
		template<class ObjectV>
		GraphicsResourceRef(const GraphicsResourceRef<ObjectV>& other) :
			_resource(other._resource), _owningDevice(other._owningDevice), _counter(other._counter)
		{
			_counter->fetch_add(1);
		}

		//template<class ObjectV, std::enable_if<std::is_convertible<ObjectT*, ObjectV*>::value, bool> = true>
		template<class ObjectV>
		GraphicsResourceRef(GraphicsResourceRef<ObjectV>&& other) :
			_owningDevice(std::move(other._owningDevice)), _resource(std::move(other._resource)), _counter(std::move(other._counter))
		{
			other._resource = nullptr;
		}

		virtual ~GraphicsResourceRef()
		{
			Invalidate();
		}

		ObjectT* Get() const { return _resource; }

		void Invalidate()
		{
			if (_counter != nullptr && _counter->fetch_sub(1) <= 1 && _resource != nullptr)
				_owningDevice->DestroyResource(_resource);

			_resource = nullptr;
		}

		void operator=(const GraphicsResourceRef& other)
		{
			Invalidate();

			_resource = other._resource;
			_counter = other._counter;
			_owningDevice = other._owningDevice;

			_counter->fetch_add(1);
		}

		ObjectT* operator->() const { return _resource; }

		//template<class ObjectV, std::enable_if<std::is_convertible<ObjectV*, ObjectT*>::value, bool> = true>
		template<class ObjectV>
		explicit operator GraphicsResourceRef() const
		{
			return GraphicsResourceRef<ObjectV>();
			//GraphicsResourceRef<ObjectV> other;
			//other._counter = _counter;
			//other._resource = static_cast<ObjectV*>(_resource);
			//other._owningDevice = _owningDevice;
			//
			//_counter->fetch_add(1);
			//
			//return other;
		}

		template<class ObjectV>
		bool operator==(const GraphicsResourceRef<ObjectV>& other) const { return _resource == other._resource; }
	};*/

	/*class GraphicsDevice;

	class COCOAPI IGraphicsResourceHandle
	{
	public:
		IGraphicsResourceHandle() = default;
		virtual ~IGraphicsResourceHandle() = default;

		/// <summary>
		/// Gets the resource
		/// </summary>
		/// <returns>The resource</returns>
		virtual void* GetHandle() const = 0;
	};

	template<class>
	class GraphicsResource;

	/// <summary>
	/// Class that manages the lifetime of a graphics resource
	/// </summary>
	/// <typeparam name="ObjectT">The type of managed object</typeparam>
	/// <typeparam name="DeleterT">An optional deleter for the managed object</typeparam>
	template<class ObjectT, typename DeleterT = std::default_delete<ObjectT>>
	class COCOAPI GraphicsResourceHandle : public IGraphicsResourceHandle
	{
		using Observer = GraphicsResource<ObjectT>;
		friend Observer;

	private:
		GraphicsDevice* _owningDevice;
		std::unique_ptr<ObjectT, DeleterT> _managedPtr;
		List<Observer*> _observers;

	public:
		GraphicsResourceHandle(GraphicsDevice* owningDevice, ObjectT* rawPtr) :
			_owningDevice(owningDevice), _managedPtr(rawPtr)
		{}

		template <class ObjectV, typename DeleterV = std::default_delete<ObjectV>, std::enable_if_t<std::is_convertible<ObjectT*, ObjectV*>::value, bool> = true>
		GraphicsResourceHandle(GraphicsResourceHandle<ObjectV, DeleterV>&& other) :
			_managedPtr(std::move(other._managedPtr)), _owningDevice(other._owningDevice)
		{
			for (uint64_t i = 0; i < other._observers.Count(); i++)
			{
				Observer** observer = other._observers[i];
				_observers.Add(&observer);
				observer->SetNewObserved(this);
			}

			// Ensure the old observers are cleared so they aren't invalidated
			other._observers.Clear();
		}

		GraphicsResourceHandle(const Managed<ObjectT, DeleterT>& other) = delete;
		GraphicsResourceHandle(ObjectT&& other) = delete;

		~GraphicsResourceHandle()
		{
			for (Observer* observer : _observers)
				observer->Invalidate();

			_observers.Clear();
			_managedPtr.reset();
		}

		virtual void* GetHandle() const override { return Get(); }

		/// <summary>
		/// Gets the device that owns this resource
		/// </summary>
		/// <returns>The owning GraphicsDevice</returns>
		GraphicsDevice* GetOwningDevice() const { return _owningDevice; }

		/// <summary>
		/// Gets the managed object
		/// </summary>
		/// <returns>The managed object</returns>
		ObjectT* Get() const { return _managedPtr.get(); }

		/// <summary>
		/// Releases the managed object without destroying it
		/// </summary>
		/// <returns>The managed object</returns>
		ObjectT* Release()
		{
			return _managedPtr.release();
		}

		/// <summary>
		/// Resets the managed object with an optional new object, destroying the old instance
		/// </summary>
		/// <param name="newPtr">The optional new object to manage</param>
		void Reset(ObjectT* newPtr = nullptr)
		{
			_managedPtr.reset(newPtr);
		}

		ObjectT* operator()() const { return _managedPtr.get(); }
		ObjectT* operator->() const { return _managedPtr.get(); }
		GraphicsResourceHandle<ObjectT, DeleterT>& operator=(const Managed<ObjectT, DeleterT>& other) = delete;
		GraphicsResourceHandle<ObjectT, DeleterT>& operator=(Managed<ObjectT, DeleterT>&& other) const { return std::move(other); }

		template <class ObjectV, typename DeleterV = std::default_delete<ObjectV>, std::enable_if_t<std::is_convertible<ObjectT*, ObjectV*>::value, bool> = true>
		operator GraphicsResourceHandle<ObjectV, DeleterV>() const { return GraphicsResourceHandle<ObjectV, DeleterV>(*this); }

	private:
		/// <summary>
		/// Adds an observer to this managed object
		/// </summary>
		/// <param name="observer">The observer</param>
		void AddObserver(Observer* observer) { _observers.Add(observer); }

		/// <summary>
		/// Removes an observer from this managed object
		/// </summary>
		/// <param name="observer">The observer</param>
		void RemoveObserver(Observer* observer) { _observers.Remove(observer); }
	};

	class COCOAPI IGraphicsResource
	{
	public:
		IGraphicsResource() = default;
		virtual ~IGraphicsResource() = default;

		/// <summary>
		/// Gets the resource that this observer observes
		/// </summary>
		/// <returns>The resource that this observer observes</returns>
		virtual IGraphicsResourceHandle* GetHandle() const = 0;

		/// <summary>
		/// Invalidates this observed resource
		/// </summary>
		virtual void Invalidate() = 0;
	};

	/// <summary>
	/// Observes a managed graphics resource and safely allows access to its object
	/// </summary>
	/// <typeparam name="ObjectT">The type of managed object</typeparam>
	template<class ObjectT>
	class COCOAPI GraphicsResource : public IGraphicsResource
	{
		using Observed = GraphicsResourceHandle<ObjectT>;
		friend Observed;

	private:
		Observed* _observed = nullptr;
		bool _isValid = false;

	public:
		GraphicsResource() : IGraphicsResource()
		{}

		GraphicsResource(Observed* observed) :
			_observed(observed), _isValid(true)
		{
			_observed->AddObserver(this);
		}

		GraphicsResource(const Observed& observed) : GraphicsResource(&observed)
		{}

		template <class ObjectV, std::enable_if_t<std::is_convertible<ObjectT*, ObjectV*>::value, bool> = true>
		GraphicsResource(GraphicsResource<ObjectV>&& other) : 
			_observed(other._observed), _isValid(true)
		{
			_observed->AddObserver(this);
		}

		~GraphicsResource()
		{
			if (_isValid)
				_observed->RemoveObserver(this);
		}

		IGraphicsResourceHandle* GetHandle() const override { return _observed; }
		virtual void Invalidate() override { _isValid = false; }

		/// <summary>
		/// Attempts to gain access to the managed object
		/// </summary>
		/// <param name="ref">Will be set to the managed object if successful</param>
		/// <returns>True if the managed object was obtained, else false</returns>
		bool Lock(ObjectT** ref)
		{
			if (!_isValid)
				return false;

			*ref = _observed->Get();
			return true;
		}

		/// <summary>
		/// Tries to get the managed object
		/// </summary>
		/// <returns>A pointer to the managed object, else nullptr if it is invalid</returns>
		ObjectT* Get() const
		{
			if (!_isValid)
				return nullptr;

			return _observed->Get();
		}

		template <class ObjectV, std::enable_if_t<std::is_base_of<ObjectV, ObjectT>::value, bool> = true>
		operator GraphicsResource<ObjectV>() const { return GraphicsResource<ObjectV>(*this); }

	private:
		void SetNewObserved(Observed* observed)
		{
			_observed = observed;
		}
	};*/
}