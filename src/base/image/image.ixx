export module image;

export import std;

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
	explicit PNGImage() noexcept = delete;
	explicit PNGImage(int w, int h, std::span<const std::uint8_t> pixelBuffer) noexcept : m_imageWidthPixels(w), m_imageHeightPixels(h), m_pixelBuffer(pixelBuffer) {}

	virtual void createImage() const override;

private:
	int m_imageWidthPixels{};
	int m_imageHeightPixels{};
	std::span<const std::uint8_t> m_pixelBuffer;
};