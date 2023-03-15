#include "CommandBuffer.h"

namespace Coco::Rendering
{
	CommandBuffer::CommandBuffer(bool isPrimary) noexcept :
		IsPrimary(isPrimary), CurrentState(State::Ready)
	{
	}

	CommandBuffer::~CommandBuffer()
	{
		CurrentState = State::NotAllocated;
	}

	void CommandBuffer::EndAndSubmit(
		const List<IGraphicsSemaphore*>& waitSemaphores,
		const List<IGraphicsSemaphore*>& signalSemaphores,
		IGraphicsFence* signalFence)
	{
		End();
		Submit(waitSemaphores, signalSemaphores, signalFence);
	}
}