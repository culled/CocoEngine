//
// Created by cullen on 3/8/26.
//

#include "Texture.h"

#include "RenderService.h"
#include <Coco/Core/Engine.h>
#include "Vendor/stbimage.h"

namespace Coco
{
    DEFINE_RTTI_TYPE(Texture, Resource);

    Texture::Texture(Engine* engine, uint64 id, const ImageDescription& imageDescription, const ImageSamplerDescription& samplerDescription) :
        Resource(engine, id),
        _image(),
        _sampler()
    {
        RenderService* rendering = engine->GetService<RenderService>();
        COCO_ASSERT(rendering, "No active RenderService found");

        GraphicsPlatform* platform = rendering->GetGraphicsPlatform();
        COCO_ASSERT(platform, "No active GraphicsPlatform found");

        _image = platform->CreateImage(imageDescription);
        _sampler = platform->CreateImageSampler(UpdateSamplerDescription(imageDescription, samplerDescription));
    }

    Texture::Texture(Engine* engine, uint64 id, const FilePath& imagePath, ImagePixelFormat pixelFormat, ImageColorSpace colorSpace,
	    const ImageSamplerDescription& samplerDescription, bool generateMipMaps) :
		Resource(engine, id),
		_image(),
		_sampler()
    {
    	RenderService* rendering = engine->GetService<RenderService>();
    	COCO_ASSERT(rendering, "No active RenderService found");

    	GraphicsPlatform* platform = rendering->GetGraphicsPlatform();
    	COCO_ASSERT(platform, "No active GraphicsPlatform found");

		_image = CreateImageFromFile(imagePath, pixelFormat, colorSpace, generateMipMaps);
    	_sampler = platform->CreateImageSampler(UpdateSamplerDescription(_image->GetDescription(), samplerDescription));
    }

    Texture::~Texture()
    {
        if (RenderService* rendering = _engine->GetService<RenderService>())
        {
            if (GraphicsPlatform* platform = rendering->GetGraphicsPlatform())
            {
                platform->InvalidateResource(_image->GetID());
                platform->InvalidateResource(_sampler->GetID());
            }
        }
    }

    void Texture::SetPixels(const void* pixelData, uint64 pixelDataSize)
    {
        _image->SetPixels(pixelData, pixelDataSize);
    }

    void Texture::Resize(uint32 newWidth, uint32 newHeight)
    {
	    ImageDescription newDescription(_image->GetDescription());
    	newDescription.Width = newWidth;
    	newDescription.Height = newHeight;

    	RenderService* rendering = _engine->GetService<RenderService>();
    	COCO_ASSERT(rendering, "RenderService hasn't been created");

    	GraphicsPlatform* graphicsPlatform = rendering->GetGraphicsPlatform();
    	COCO_ASSERT(graphicsPlatform, "No active GraphicsPlatform found");

    	graphicsPlatform->InvalidateResource(_image->GetID());
    	_image = graphicsPlatform->CreateImage(newDescription);
    }

    ImageSamplerDescription Texture::UpdateSamplerDescription(const ImageDescription& imageDesc,
                                                              const ImageSamplerDescription& samplerDesc)
    {
	    ImageSamplerDescription r(samplerDesc);
    	r.MaxLOD = imageDesc.MipCount - 1;
    	return r;
    }

    Ref<Image> Texture::CreateImageFromFile(const FilePath& imagePath, ImagePixelFormat pixelFormat,
                                            ImageColorSpace colorSpace, bool generateMipMaps)
    {
	    RenderService* rendering = _engine->GetService<RenderService>();
		COCO_ASSERT(rendering, "RenderService hasn't been created");

    	GraphicsPlatform* graphicsPlatform = rendering->GetGraphicsPlatform();
    	COCO_ASSERT(graphicsPlatform, "No active GraphicsPlatform found");

		// Set Y = 0 to the top
		stbi_set_flip_vertically_on_load(true);

		// Load in the image data from the file
		int channelsToLoad = ImageDescription::GetChannelCount(pixelFormat);
		int actualChannelCount;
		int width, height;

    	Engine* engine = Engine::Get();
		File f = engine->GetFileSystem()->Open(imagePath, FileOpenFlags::Read, false);
		Array<uint8> fileData;
		f.ReadToEnd(fileData);
		f.Close();

		uint8* rawImageData = stbi_load_from_memory(fileData.Data(), static_cast<int>(fileData.GetCount()), &width, &height, &actualChannelCount, channelsToLoad);

		if (rawImageData == nullptr || stbi_failure_reason())
		{
			COCO_ENGINE_LOG_ERROR("Failed to load image data from \"%s\": %s", imagePath.CStr(), stbi_failure_reason());

			// Free the image data if it was somehow loaded
			if (rawImageData)
			{
				stbi_image_free(rawImageData);
				rawImageData = nullptr;
			}

			// Clears any error so it won't cause subsequent false failures
			stbi_clear_error();

			return Ref<Image>();
		}

		ImageDescription imageDesc(
			static_cast<uint32>(width), static_cast<uint32>(height),
			pixelFormat,
			colorSpace,
			ImageUsageFlags::Sampled,
			generateMipMaps);

		uint64 byteSize = static_cast<uint64_t>(width) * height * channelsToLoad;

		// Check if there is any transparency in the image
		//bool hasTransparency = false;
		//double averageMagnitude = 0.0;
		//
		//for (uint64_t i = 0; i < byteSize; i += channelsToLoad)
		//{
		//	if (actualChannelCount > 3 && rawImageData[i + 3] < 255)
		//	{
		//		hasTransparency = true;
		//	}
		//
		//	double x = (rawImageData[i] - 127.0) / 128.0;
		//	double y = (rawImageData[i + 1] - 127.0) / 128.0;
		//	Vector3 rgb(x, y, rawImageData[i + 2] / 255.0);
		//	averageMagnitude += rgb.GetLength();
		//}
		//
		//// If this value is very close to 1.0, then the texture is likely to be a normal map
		//averageMagnitude /= static_cast<double>(width) * height;
		//
		//if (Math::Approximately(averageMagnitude, 1.0, 0.02))
		//{
		//	// TODO: add more automatic normalmap detection, like compression
		//	imageDesc.ColorSpace = ImageColorSpace::Linear;
		//}

    	Ref<Image> image;
    	try
    	{
    		image = graphicsPlatform->CreateImage(imageDesc);
    		image->SetPixels(rawImageData, byteSize);
    		stbi_image_free(rawImageData);
    	} catch (const Exception& ex)
    	{
    		COCO_ENGINE_LOG_ERROR("Failed to transfer image data into backend: %s", ex.what());

    		stbi_image_free(rawImageData);

    		return image;
    	}

		COCO_ENGINE_LOG_VERBOSE("Created image %u from file \"%s\"", image->GetID(), imagePath.CStr());
    	return image;
    }
} // Coco