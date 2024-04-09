import image;

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../dep/stb_image_write.h"

void PNGImage::createImage() const
{
	stbi_write_png("output.png", m_imageWidthPixels, m_imageHeightPixels, 3, m_pixelBuffer.data(), m_imageWidthPixels * 3);
}