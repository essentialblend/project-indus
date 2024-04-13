import image;

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../dep/stb_image_write.h"

PNGImage::PNGImage(const ImageProperties& imgProp, std::span<Color const> pixelBuffer) noexcept : m_imageProperties{ imgProp }, m_baseBuffer{ std::vector<Color>(pixelBuffer.begin(), pixelBuffer.end()) } {}

void PNGImage::createImage()
{
	for (auto& color : m_baseBuffer)
	{
		Color converted{ color.convertNormToIntegral() }; // Works
		
		m_finalBuffer.push_back(static_cast<std::uint8_t>(converted.getBaseVec()[0]));
		m_finalBuffer.push_back(static_cast<std::uint8_t>(converted.getBaseVec()[1]));
		m_finalBuffer.push_back(static_cast<std::uint8_t>(converted.getBaseVec()[2]));
	}
	stbi_write_png("output.png", m_imageProperties.pixelResolutionObj.widthInPixels, m_imageProperties.pixelResolutionObj.heightInPixels, m_imageProperties.numColorChannels, m_finalBuffer.data(), m_imageProperties.pixelResolutionObj.widthInPixels * m_imageProperties.numColorChannels);
}