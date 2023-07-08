#include "CommandBuffer.h"
#include "GraphicsSemaphore.h"
#include "GraphicsFence.h"

namespace Coco::Rendering
{
	CommandBuffer::CommandBuffer(ResourceID id, const string& name, bool isPrimary) noexcept : RenderingResource(id, name),
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

	void CommandBuffer::Submit(const List<GraphicsSemaphore*>& waitSemaphores, const List<GraphicsSemaphore*>& signalSemaphores, GraphicsFence* signalFence)
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
		const List<GraphicsSemaphore*>& waitSemaphores,
		const List<GraphicsSemaphore*>& signalSemaphores,
		GraphicsFence* signalFence)
	{
		End();
		Submit(waitSemaphores, signalSemaphores, signalFence);
	}
}