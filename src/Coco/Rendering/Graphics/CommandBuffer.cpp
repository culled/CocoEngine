#include "CommandBuffer.h"

namespace Coco::Rendering
{
	CommandBuffer::CommandBuffer(bool isPrimary) noexcept :
		IsPrimary(isPrimary), CurrentState(CommandBufferState::Ready)
	{
	}

	void CommandBuffer::Begin(bool isSingleUse, bool isSimultaneousUse)
	{
		BeginImpl(isSingleUse, isSimultaneousUse);
		CurrentState = CommandBufferState::Recording;
	}

	void CommandBuffer::End()
	{
		EndImpl();
		CurrentState = CommandBufferState::RecordingEnded;
	}

	void CommandBuffer::Submit(const List<IGraphicsSemaphore*>& waitSemaphores, const List<IGraphicsSemaphore*>& signalSemaphores, IGraphicsFence* signalFence)
	{
		SubmitImpl(waitSemaphores, signalSemaphores, signalFence);
		CurrentState = CommandBufferState::Submitted;
	}

	void CommandBuffer::Reset()
	{
		ResetImpl();
		CurrentState = CommandBufferState::Ready;
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