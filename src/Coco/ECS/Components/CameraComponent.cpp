#include "CameraComponent.h"

#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/Graphics/Resources/Image.h>
#include <Coco/Rendering/RenderingService.h>

namespace Coco::ECS
{
	ImageCache::ImageCache(const Ref<Rendering::RenderPipeline>& pipeline) : CachedResource(pipeline->ID, pipeline->GetVersion()),
		PipelineRef(pipeline)
	{}

	bool ImageCache::NeedsUpdate() const noexcept
	{
		if (Ref<Rendering::RenderPipeline> pipeline = PipelineRef.lock())
		{
			return pipeline->GetVersion() == GetVersion() ||
				Images.size() == 0 ||
				std::any_of(Images.cbegin(), Images.cend(), [](const auto& it) {
					return !it.second.IsValid();
					});
		}
		return false;
	}

	void ImageCache::Update(const UnorderedMap<int, WeakManagedRef<Rendering::Image>>& images)
	{
		Images = images;
		
		UpdateVersion(PipelineRef.lock()->GetVersion());
	}

	CameraComponent::CameraComponent(EntityID ownerID) : EntityComponent(ownerID)
	{}

	void CameraComponent::SetPerspectiveProjection(double fieldOfView, double aspectRatio, double nearClipDistance, double farClipDistance) noexcept
	{
		_projectionType = ProjectionType::Perspective;
		_perspectiveFieldOfView = fieldOfView;
		_nearClipDistance = nearClipDistance;
		_farClipDistance = farClipDistance;

		_isProjectionMatrixDirty = true;
	}

	void CameraComponent::SetOrthographicProjection(double size, double aspectRatio, double nearClipDistance, double farClipDistance) noexcept
	{
		_projectionType = ProjectionType::Orthographic;
		_orthographicSize = size;
		_nearClipDistance = nearClipDistance;
		_farClipDistance = farClipDistance;

		_isProjectionMatrixDirty = true;
	}

	void CameraComponent::SetCustomProjection(const Matrix4x4& projection) noexcept
	{
		_projectionType = ProjectionType::Custom;
		_projectionMatrix = projection;

		_isProjectionMatrixDirty = false;
	}

	const Matrix4x4& CameraComponent::GetProjectionMatrix() noexcept
	{
		if (_isProjectionMatrixDirty)
			UpdateProjectionMatrix();

		return _projectionMatrix;
	}

	void CameraComponent::SetAspectRatio(double aspectRatio) noexcept
	{
		if (Math::Approximately(aspectRatio, _aspectRatio))
			return;

		_aspectRatio = aspectRatio;

		_isProjectionMatrixDirty = true;
	}

	void CameraComponent::SetNearClipDistance(double nearClipDistance) noexcept
	{
		if (Math::Approximately(nearClipDistance, _nearClipDistance))
			return;

		_nearClipDistance = Math::Max(nearClipDistance, Math::Epsilon);

		_isProjectionMatrixDirty = true;
	}

	void CameraComponent::SetFarClipDistance(double farClipDistance) noexcept
	{
		if (Math::Approximately(farClipDistance, _farClipDistance))
			return;

		_farClipDistance = Math::Max(farClipDistance, Math::Epsilon);

		_isProjectionMatrixDirty = true;
	}

	List<WeakManagedRef<Rendering::Image>> CameraComponent::GetRenderTargets(const Ref<Rendering::RenderPipeline>& pipeline, const SizeInt& size)
	{
		const List<Rendering::RenderPipelineAttachmentDescription>& attachments = pipeline->GetPipelineAttachmentDescriptions();

		// Get the cached images for this pipeline
		if (!_imageCache.contains(pipeline->ID))
			_imageCache.emplace(pipeline->ID, CreateRef<ImageCache>(ImageCache(pipeline)));

		Ref<ImageCache>& resource = _imageCache.at(pipeline->ID);
		resource->UpdateTickUsed();

		List<int> overrideMappings(_renderTargetOverrides.Count());
		List<WeakManagedRef<Rendering::Image>> renderTargets(attachments.Count());
		UnorderedMap<int, WeakManagedRef<Rendering::Image>>& generatedImages = resource->Images;

		for (int i = 0; i < _renderTargetOverrides.Count(); i++)
			overrideMappings[i] = i;

		for (int i = 0; i < attachments.Count(); i++)
		{
			const Rendering::RenderPipelineAttachmentDescription& pipelineAttachment = attachments[i];

			// Try to find an override that matches the needed attachment
			for (auto it = overrideMappings.begin(); it != overrideMappings.end(); it++)
			{
				const WeakManagedRef<Rendering::Image>& rtOverride = _renderTargetOverrides[*it];

				if (!rtOverride.IsValid())
				{
					it = overrideMappings.EraseAndGetNext(it);
					continue;
				}

				if (pipelineAttachment.Description.PixelFormat == rtOverride->GetDescription().PixelFormat &&
					pipelineAttachment.Description.ColorSpace == rtOverride->GetDescription().ColorSpace)
				{
					renderTargets[i] = rtOverride;
					it = overrideMappings.EraseAndGetNext(it);
					break;
				}
			}

			if (!renderTargets[i].IsValid())
			{
				Rendering::ImageDescription attachmentDescription(
					size.Width, size.Height,
					1,
					pipelineAttachment.Description.PixelFormat,
					pipelineAttachment.Description.ColorSpace,
					Rendering::ImageUsageFlags::RenderTarget | Rendering::ImageUsageFlags::Sampled | Rendering::ImageUsageFlags::TransferSource
				);

				bool generateImage = true;

				// No render target override for this attachment, so use a cached one (it if exists)
				if (generatedImages.contains(i))
				{
					const WeakManagedRef<Rendering::Image>& image = generatedImages.at(i);
					if (image.IsValid() && image->GetDescription() == attachmentDescription)
					{
						// Reuse a cached image
						renderTargets[i] = image;
						generateImage = false;
					}
				}
				
				if(generateImage)
				{
					renderTargets[i] = EnsureRenderingService()->GetPlatform()->CreateImage(attachmentDescription);

					generatedImages[i] = renderTargets[i];
				}
			}
		}

		return renderTargets;
	}

	Ref<Rendering::RenderView> CameraComponent::GetRenderView(
		const Ref<Rendering::RenderPipeline>& pipeline,
		const SizeInt& backbufferSize,
		const List<WeakManagedRef<Rendering::Image>>& backbuffers)
	{
		// Make sure the camera matches our rendering aspect ratio
		SetAspectRatio(static_cast<double>(backbufferSize.Width) / backbufferSize.Height);

		SetRenderTargetOverrides(backbuffers);

		return CreateRef<Rendering::RenderView>(
			RectInt(Vector2Int::Zero, backbufferSize),
			pipeline->GetClearColor(),
			GetProjectionMatrix(),
			GetViewMatrix(),
			GetRenderTargets(pipeline, backbufferSize));
	}

	void CameraComponent::UpdateProjectionMatrix() noexcept
	{
		if (_projectionType == ProjectionType::Custom || !_isProjectionMatrixDirty)
			return;
		
		if (_projectionType == ProjectionType::Perspective)
		{
			_projectionMatrix = Matrix4x4::CreatePerspectiveProjection(
				Math::Deg2Rad(_perspectiveFieldOfView), 
				_aspectRatio, 
				_nearClipDistance, 
				_farClipDistance);
		}
		else
		{
			_projectionMatrix = Matrix4x4::CreateOrthographicProjection(
				_orthographicSize, 
				_aspectRatio, 
				_nearClipDistance, 
				_farClipDistance);
		}

		_isProjectionMatrixDirty = false;
	}

	Rendering::RenderingService* CameraComponent::EnsureRenderingService() const
	{
		Rendering::RenderingService* renderService = Rendering::RenderingService::Get();

		if (renderService == nullptr)
			throw Exception("No active rendering service found");

		return renderService;
	}
}