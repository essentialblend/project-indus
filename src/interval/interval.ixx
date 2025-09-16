export module interval;

import std;
import types;

export class Interval
{
public:
	constexpr explicit Interval() noexcept;
	constexpr explicit Interval(Float, Float) noexcept;
	
	[[nodiscard]] constexpr Float getMin() const noexcept;
	[[nodiscard]] constexpr Float getMax() const noexcept;
	[[nodiscard]] constexpr Float getSpan() const noexcept;

	[[nodiscard]] constexpr bool isContained(Float) const noexcept;
	[[nodiscard]] constexpr bool isSurrounded(Float) const noexcept;

	[[nodiscard]] constexpr Float clampWithin(Float) const noexcept;
	
private:
	Float m_minInterval{};
	Float m_maxInterval{};
	static const Interval m_emptyInterval;
	static const Interval m_universalInterval;
};

constexpr Interval::Interval() noexcept : m_minInterval(+std::numeric_limits<Float>::infinity()), m_maxInterval(-std::numeric_limits<Float>::infinity()) {}

constexpr Interval::Interval(Float min, Float max) noexcept : m_minInterval(min), m_maxInterval(max) {}

constexpr Float Interval::getMin() const noexcept
{
	return m_minInterval;
}

constexpr Float Interval::getMax() const noexcept
{
	return m_maxInterval;
}

constexpr Float Interval::getSpan() const noexcept
{
	return m_maxInterval - m_minInterval;
}

constexpr bool Interval::isContained(Float value) const noexcept
{
	return value >= m_minInterval && value <= m_maxInterval;
}

constexpr bool Interval::isSurrounded(Float value) const noexcept
{
	return value > m_minInterval && value < m_maxInterval;
}

constexpr Float Interval::clampWithin(Float x) const noexcept
{
	return std::clamp(x, m_minInterval, m_maxInterval);
}