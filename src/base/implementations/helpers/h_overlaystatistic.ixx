export module h_overlaystatistic;

import <string>;

export class HOverlayStatistic
{
public:
    explicit HOverlayStatistic() noexcept = default;
    explicit HOverlayStatistic(const std::string_view titleStr, const std::string_view resultStr) noexcept
        : m_titleStr(titleStr), m_resultStr(resultStr) {}

    const std::string& getTitle() const noexcept { return m_titleStr; }
    const std::string& getResult() const noexcept { return m_resultStr; }

    void setTitle(const std::string_view titleStr) noexcept { m_titleStr = titleStr; }
    void setResult(const std::string_view resultStr) noexcept { m_resultStr = resultStr; }

    ~HOverlayStatistic() noexcept = default;

private:
    std::string m_titleStr;
    std::string m_resultStr;
};
