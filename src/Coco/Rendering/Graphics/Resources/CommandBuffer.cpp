#include "CommandBuffer.h"
#include "GraphicsSemaphore.h"
#include "GraphicsFence.h"

namespace Coco::Rendering
{
	CommandBuffer::CommandBuffer(ResourceID id, const string& name, bool isPrimary) noexcept : RenderingResource(id, name),
		IsPrimary(isPrimary), _currentState(CommandBufferState::Ready)
	{
	}

	void CommandBuffer::Begin(bool isSingleUse, bool isSimultaneousUse)
	{
		BeginImpl(isSingleUse, isSimultaneousUse);
		_currentState = CommandBufferState::Recording;
	}

	void CommandBuffer::End()
	{
		EndImpl();
		_currentState = CommandBufferState::RecordingEnded;
	}

	void CommandBuffer::Submit(List<GraphicsSemaphore*>* waitSemaphores, List<GraphicsSemaphore*>* signalSemaphores, GraphicsFence* signalFence)
	{
		SubmitImpl(waitSemaphores, signalSemaphores, signalFence);
		_currentState = CommandBufferState::Submitted;
	}

	void CommandBuffer::Reset()
	{
		ResetImpl();
		_currentState = CommandBufferState::Ready;
	}

	void CommandBuffer::EndAndSubmit(
		List<GraphicsSemaphore*>* waitSemaphores,
		List<GraphicsSemaphore*>* signalSemaphores,
		GraphicsFence* signalFence)
	{
		End();
		Submit(waitSemaphores, signalSemaphores, signalFence);
	}
}