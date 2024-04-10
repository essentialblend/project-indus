export module image;
import std;
import color;

export class Image abstract
{
public:
	explicit Image() = default;
	virtual void createImage() const = 0;
	
	virtual ~Image() noexcept = default;
};

export class PNGImage : public Image
{
public:
	explicit PNGImage() noexcept = default;
	explicit PNGImage(int w, int h, int numChannels, std::span<Color const> pixelBuffer) noexcept;

	virtual void createImage() const override;

private:
	int m_imageWidthPixels{};
	int m_imageHeightPixels{};
	int m_numImgChannels{};
	std::span<Color const> m_pixelBuffer{};

};