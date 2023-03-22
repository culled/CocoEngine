#pragma once

#include <Coco/Core/Resources/Loaders/KeyValueResourceLoader.h>

namespace Coco::Rendering
{
	/// <summary>
	/// A loader for texture files (*.ctexture)
	/// </summary>
	class TextureLoader final : public KeyValueResourceLoader
	{
	private:
		static constexpr const char* s_imageFileVariable = "imageFile";
		static constexpr const char* s_usageFlagsVariable = "usageFlags";
		static constexpr const char* s_filterModeVariable = "filterMode";
		static constexpr const char* s_repeatModeVariable = "repeatMode";
		static constexpr const char* s_maxAnisotropyVariable = "maxAnisotropy";
		static constexpr const char* s_channelCountVariable = "channelCount";

	public:
		TextureLoader(const string& basePath);
		~TextureLoader() final = default;

		const char* GetResourceTypename() const noexcept final { return ResourceTypeToString(ResourceType::Texture); }

	protected:
		Ref<Resource> LoadImpl(const string& path) final;
		void SaveImpl(const Ref<Resource>& resource, const string& path) final;
	};
}