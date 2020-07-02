#ifndef __TEXTURE_ASSET_HPP_
#define __TEXTURE_ASSET_HPP_

#include <memory>
#include <cstdint>

#include <stb/stb_image.h>

namespace asset
{
	template <typename T>
	struct texture_asset_template
	{
		texture_asset_template(
			int p_width,
			int p_height,
			int p_channels,
			T* p_data
		) :
			width(p_width),
			height(p_height),
			channels(p_channels),
			data(p_data, stbi_image_free)
		{}


		int width;
		int height;
		int channels;
		std::unique_ptr<T, decltype(&stbi_image_free)> data;
	};

	typedef texture_asset_template<unsigned char> texture_asset;
	typedef texture_asset_template<float> texture_assetf;
}

#endif