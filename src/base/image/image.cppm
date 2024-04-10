import image;

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../dep/stb_image_write.h"

PNGImage::PNGImage(int w, int h, int numChannels, std::span<Color const> pixelBuffer) noexcept : m_imageWidthPixels(w), m_imageHeightPixels(h), m_numImgChannels(numChannels), m_pixelBuffer(pixelBuffer) {}

void PNGImage::createImage() const
{
	std::vector<std::uint8_t> finalPNGBuffer{};

	for (auto& color : m_pixelBuffer)
	{
		Color converted{ color.convertNormToIntegral() }; // Works
		
		finalPNGBuffer.push_back(static_cast<std::uint8_t>(converted.getBaseVec()[0]));
		finalPNGBuffer.push_back(static_cast<std::uint8_t>(converted.getBaseVec()[1]));
		finalPNGBuffer.push_back(static_cast<std::uint8_t>(converted.getBaseVec()[2]));
	}
	stbi_write_png("output.png", m_imageWidthPixels, m_imageHeightPixels, m_numImgChannels, finalPNGBuffer.data(), m_imageWidthPixels * m_numImgChannels);
}