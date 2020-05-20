#ifndef __TEXTURE_ASSET_HPP_
#define __TEXTURE_ASSET_HPP_

#include <memory>
#include <cstdint>
#include <stb/stb_image.h>

namespace asset
{
	struct texture_asset
	{
		texture_asset(
			int p_width,
			int p_height,
			int p_channels,
			unsigned char* p_data
		) :
			width(p_width),
			height(p_height),
			channels(p_channels),
			data(p_data, stbi_image_free)
		{}


		int width;
		int height;
		int channels;
		std::unique_ptr<unsigned char, decltype(&stbi_image_free)> data;
	};
}

#endif