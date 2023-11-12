#pragma once

#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/MainLoop/TickListener.h>

namespace Coco
{
	class ResourceDialog
	{
	protected:
		bool _isOpen;
		ManagedRef<TickListener> _tickListener;

	protected:
		ResourceDialog();

	public:
		~ResourceDialog();

		void Open();
		void Close();

	protected:
		virtual void Opened() {}
		virtual void Closed() {}
		virtual const char* GetPopupName() const = 0;
		virtual void Draw() = 0;

	private:
		void HandleTick(const TickInfo& tick);
	};
}