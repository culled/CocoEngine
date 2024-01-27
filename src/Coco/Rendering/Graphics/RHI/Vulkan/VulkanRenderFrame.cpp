#include "Renderpch.h"
#include "VulkanRenderFrame.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanGraphicsFence.h"
#include "VulkanGraphicsSemaphore.h"
#include "VulkanPresenter.h"
#include "VulkanImage.h"
#include "VulkanRenderContext.h"
#include "../../../Providers/RenderViewDataProvider.h"
#include "../../../Providers/SceneDataProvider.h"
#include "../../../Mesh.h"
#include "../../../Material.h"
#include "../../../Pipeline/CompiledRenderPipeline.h"
#include "../../../Pipeline/RenderPipeline.h"
#include "../../../RenderService.h"

namespace Coco::Rendering::Vulkan
{
	VulkanRenderFrameRender::VulkanRenderFrameRender(Ref<VulkanRenderContext> context, SharedRef<RenderView> view) :
		Context(context),
		View(view),
		WaitSemaphores(),
		SignalSemaphores()
	{}

	VulkanRenderFrame::VulkanRenderFrame(VulkanGraphicsDevice& device) :
		_device(device),
		_graphicsQueue(device.GetQueue(VulkanQueueType::Graphics)),
		_meshCache(CreateUniqueRef<VulkanMeshCache>(device, true)),
		_uniformCache(CreateUniqueRef<VulkanUniformCache>(device)),
		_lastPresenterRenderIndices(),
		_unusedRenderSemaphores(),
		_renderSemaphores(),
		_unusedRenderFences(),
		_renderFences()
	{
		_presentCommandBuffer = CreateUniqueRef<VulkanCommandBuffer>(_graphicsQueue->GetCommandPool()->Allocate(true));
	}

	VulkanRenderFrame::~VulkanRenderFrame()
	{
		WaitForRenderToComplete();
		DestroyFrameObjects();
		_unusedRenderFences.clear();
		_unusedRenderSemaphores.clear();

		_graphicsQueue->GetCommandPool()->Free(*_presentCommandBuffer);
		_presentCommandBuffer.reset();

		_uniformCache.reset();
		_meshCache.reset();
	}

	void VulkanRenderFrame::WaitForRenderToComplete()
	{
		for (auto& render : _renders)
			render.Context->WaitForRenderToComplete();

		for (auto& fence : _renderFences)
		{
			if(!fence->IsSignaled())
				fence->WaitForSignal(Math::MaxValue<uint64>());
		}
	}

	bool VulkanRenderFrame::IsRenderComplete() const
	{
		for (auto& render : _renders)
			if (!render.Context->IsIdle())
				return false;

		for (auto& fence : _renderFences)
			if (!fence->IsSignaled())
				return false;

		return true;
	}

	bool VulkanRenderFrame::Render(
		uint64 rendererID,
		Ref<Presenter> presenter,
		const CompiledRenderPipeline& pipeline, 
		RenderViewDataProvider& renderViewDataProvider, 
		std::span<SceneDataProvider*> sceneDataProviders)
	{
		Ref<VulkanPresenter> vulkanPresenter = StaticRefCast<VulkanPresenter>(presenter);

		Ref<VulkanGraphicsSemaphore> acquireImageSemaphore;
		Ref<VulkanImage> framebuffer;
		if (!AddPresenter(vulkanPresenter, acquireImageSemaphore, framebuffer))
			return false;

		std::array<Ref<Image>, 1> framebuffers = { framebuffer };

		Ref<VulkanRenderContext> context = GetRenderContext();
		VulkanRenderFrameRender& render = _renders.emplace_back(
			context,
			CreateRenderView(rendererID, framebuffers, presenter->GetFramebufferSize(), pipeline, renderViewDataProvider, sceneDataProviders)
		);

		_lastPresenterRenderIndices[vulkanPresenter->ID] = _renders.size() - 1;

		if (acquireImageSemaphore)
			render.WaitSemaphores.push_back(acquireImageSemaphore);

		ExecuteRender(pipeline, render);

		return true;
	}

	bool VulkanRenderFrame::Render(
		uint64 rendererID, 
		std::span<Ref<Image>> framebuffers,
		const SizeInt& framebufferSize,
		const CompiledRenderPipeline& pipeline, 
		RenderViewDataProvider& renderViewDataProvider, 
		std::span<SceneDataProvider*> sceneDataProviders)
	{
		Ref<VulkanRenderContext> context = GetRenderContext();

		VulkanRenderFrameRender& render = _renders.emplace_back(
			context,
			CreateRenderView(rendererID, framebuffers, framebufferSize, pipeline, renderViewDataProvider, sceneDataProviders)
		);

		ExecuteRender(pipeline, render);

		return true;
	}

	void VulkanRenderFrame::UploadMesh(const SharedRef<Mesh>& mesh)
	{
		const CachedVulkanMesh& cachedMesh = _meshCache->CreateOrUpdateMesh(mesh);

		if ((mesh->GetUsageFlags() & MeshUsageFlags::Dynamic) == MeshUsageFlags::Dynamic)
			return;

		VulkanMeshCache& deviceMeshCache = _device.GetCache().GetMeshCache();
		deviceMeshCache.CopyMesh(cachedMesh);
	}

	bool VulkanRenderFrame::TryGetCachedMesh(const Mesh& mesh, const CachedVulkanMesh*& outCachedMesh) const
	{
		VulkanMeshCache* meshCache;

		if ((mesh.GetUsageFlags() & MeshUsageFlags::Dynamic) == MeshUsageFlags::Dynamic)
			meshCache = _meshCache.get();
		else
			meshCache = &_device.GetCache().GetMeshCache();
		
		return meshCache->TryGetMesh(mesh.GetID(), outCachedMesh);
	}

	void VulkanRenderFrame::Reset()
	{
		WaitForRenderToComplete();
		DestroyFrameObjects();

		_presentCommandBuffer->Reset();
		_meshCache->Clear();

		// TODO: make this configurable
		const double _staleThreshold = 2.0;
		_uniformCache->PurgeUnusedUniformData(_staleThreshold);
	}

	void VulkanRenderFrame::SubmitRenderTasks()
	{
		if (_presenters.size() > 0)
			PresentImages();
	}

	bool VulkanRenderFrame::AddPresenter(Ref<VulkanPresenter> presenter, Ref<VulkanGraphicsSemaphore>& outImageAcquiredSemaphore, Ref<VulkanImage>& outImage)
	{
		auto it = std::find(_presenters.begin(), _presenters.end(), presenter);

		if (it != _presenters.end())
		{
			outImage = (*it)->GetAcquiredImage();
			return true;
		}

		outImageAcquiredSemaphore = GetSemaphore();

		if (!presenter->AcquireImage(outImageAcquiredSemaphore, outImage))
		{
			return false;
		}

		_presenters.push_back(presenter);

		return true;
	}

	Ref<VulkanRenderContext> VulkanRenderFrame::GetRenderContext()
	{
		return _device.GetCache().GetRenderContextPool().Get();
	}

	Ref<VulkanGraphicsSemaphore> VulkanRenderFrame::GetSemaphore()
	{
		if (_unusedRenderSemaphores.size() > 0)
		{
			_renderSemaphores.emplace_back(std::move(_unusedRenderSemaphores.front()));
			_unusedRenderSemaphores.erase(_unusedRenderSemaphores.begin());
			return _renderSemaphores.back();
		}

		return _renderSemaphores.emplace_back(CreateManagedRef<VulkanGraphicsSemaphore>(0, _device));
	}

	Ref<VulkanGraphicsFence> VulkanRenderFrame::GetFence()
	{
		if (_unusedRenderFences.size() > 0)
		{
			_renderFences.emplace_back(std::move(_unusedRenderFences.front()));
			_unusedRenderFences.erase(_unusedRenderFences.begin());
			Ref<VulkanGraphicsFence> fence = _renderFences.back();
			fence->Reset();
			return fence;
		}

		return _renderFences.emplace_back(CreateManagedRef<VulkanGraphicsFence>(0, _device, false));
	}

	void VulkanRenderFrame::ExecuteRender(const CompiledRenderPipeline& renderPipeline, VulkanRenderFrameRender& render)
	{
		// Go through each pass and execute it
		for (auto it = renderPipeline.RenderPasses.begin(); it != renderPipeline.RenderPasses.end(); it++)
		{
			if (it == renderPipeline.RenderPasses.begin())
			{
				render.Context->Begin(renderPipeline, *render.View, *this);
				render.Context->SetGlobalUniforms(render.View->GetGlobalUniformValues());
			}
			else
			{
				render.Context->BeginNextPass();
			}

			it->Pass->Execute(*render.Context, *render.View);
		}

		std::vector<std::pair<Ref<VulkanGraphicsSemaphore>, VkPipelineStageFlags>> waits;

		for (const auto& waitSem : render.WaitSemaphores)
		{
			waits.emplace_back(waitSem, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
		}

		render.Context->End(waits, render.SignalSemaphores);
	}

	void VulkanRenderFrame::PresentImages()
	{
		_presentCommandBuffer->Begin(true, false);

		VulkanQueue* presentQueue = _device.GetQueue(VulkanQueueType::Present);

		std::vector<Ref<VulkanGraphicsSemaphore>> signalSemaphores;
		signalSemaphores.reserve(_presenters.size());

		// Transition each framebuffer
		for (uint64 i = 0; i < _presenters.size(); i++)
		{
			Ref<VulkanPresenter>& presenter = _presenters.at(i);
			Ref<VulkanImage> framebuffer = presenter->GetAcquiredImage();

			framebuffer->TransitionLayout(*_presentCommandBuffer, *_graphicsQueue, *presentQueue, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
			signalSemaphores.push_back(GetSemaphore());
		}

		_presentCommandBuffer->EndAndSubmit(std::span<std::pair<Ref<VulkanGraphicsSemaphore>, VkPipelineStageFlags>>(), signalSemaphores, GetFence());

		// Present each presenter
		for (uint64 i = 0; i < _presenters.size(); i++)
		{
			Ref<VulkanPresenter>& presenter = _presenters.at(i);
			Ref<VulkanGraphicsSemaphore> semaphore = signalSemaphores.at(i);

			presenter->Present(semaphore);
		}
	}

	void VulkanRenderFrame::DestroyFrameObjects()
	{
		_renders.clear();

		for (auto it = _renderSemaphores.begin(); it != _renderSemaphores.end(); ++it)
		{
			_unusedRenderSemaphores.emplace_back(std::move(*it));
		}

		_renderSemaphores.clear();

		for (auto it = _renderFences.begin(); it != _renderFences.end(); ++it)
		{
			_unusedRenderFences.emplace_back(std::move(*it));
		}

		_renderFences.clear();

		_lastPresenterRenderIndices.clear();
		_presenters.clear();
	}
}