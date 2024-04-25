import interval;

import <algorithm>;

import core_util;

Interval::Interval() noexcept : m_minInterval(+UInfinity), m_maxInterval(-UInfinity) {}

Interval::Interval(double min, double max) noexcept : m_minInterval(min), m_maxInterval(max) {}

double Interval::getMin() const noexcept
{
	return m_minInterval;
}

double Interval::getMax() const noexcept
{
	return m_maxInterval;
}

double Interval::getSpan() const noexcept
{
	return m_maxInterval - m_minInterval;
}

bool Interval::isContained(double value) const noexcept
{
	return value >= m_minInterval && value <= m_maxInterval;
}

bool Interval::isSurrounded(double value) const noexcept
{
	return value > m_minInterval && value < m_maxInterval;
}

double Interval::clampWithin(double x) const noexcept
{
	return std::clamp(x, m_minInterval, m_maxInterval);
}