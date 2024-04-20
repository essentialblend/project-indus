export module image;

import <vector>;
import <span>;

import color;
import core_constructs;

export class Image abstract
{
public:
	explicit Image() = default;
	virtual void createImage() = 0;
	
	virtual ~Image() noexcept = default;
};

export class PNGImage : public Image
{

public:
	explicit PNGImage() noexcept = default;
	explicit PNGImage(const ImageProperties& imgProp, std::span<Color const> pixelBuffer) noexcept;

	[[noreturn]] virtual void createImage() override;

private:
	ImageProperties m_imageProperties{};
	std::vector<Color> m_baseBuffer{};
	std::vector<std::uint8_t> m_finalBuffer{};
};