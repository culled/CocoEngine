#pragma once

#include <Coco/Core/Resources/Loaders/KeyValueResourceLoader.h>

namespace Coco::Rendering
{
	/// @brief A loader for texture files (*.ctexture)
	class TextureLoader final : public KeyValueResourceLoader
	{
	private:
		static constexpr const char* s_nameVariable = "name";
		static constexpr const char* s_imageFileVariable = "imageFile";
		static constexpr const char* s_usageFlagsVariable = "usageFlags";
		static constexpr const char* s_filterModeVariable = "filterMode";
		static constexpr const char* s_repeatModeVariable = "repeatMode";
		static constexpr const char* s_maxAnisotropyVariable = "maxAnisotropy";
		static constexpr const char* s_channelCountVariable = "channelCount";

	public:
		TextureLoader(ResourceLibrary* library);
		~TextureLoader() final = default;

		string GetResourceTypename() const noexcept final { return ResourceTypeToString(ResourceType::Texture); }

	protected:
		Ref<Resource> LoadImpl(const string& path) final;
		void SaveImpl(const Ref<Resource>& resource, const string& path) final;
	};
}