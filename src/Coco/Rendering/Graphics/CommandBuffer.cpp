#include "CommandBuffer.h"

namespace Coco::Rendering
{
	CommandBuffer::CommandBuffer(bool isPrimary) :
		IsPrimary(isPrimary), CurrentState(State::Ready)
	{
	}

	CommandBuffer::~CommandBuffer()
	{
		CurrentState = State::NotAllocated;
	}

	void CommandBuffer::EndAndSubmit(const List<GraphicsSemaphore*>& waitSemaphores, const List<GraphicsSemaphore*>& signalSemaphores, GraphicsFence* signalFence)
	{
		End();
		Submit(waitSemaphores, signalSemaphores, signalFence);
	}
}