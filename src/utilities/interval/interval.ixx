export module interval;

export class Interval
{
public:
	explicit Interval() noexcept;
	explicit Interval(double min, double max) noexcept;
	
	double getMin() const noexcept;
	double getMax() const noexcept;
	double getSpan() const noexcept;
	
	bool isContained(double value) const noexcept;
	bool isSurrounded(double value) const noexcept;

	double clampWithin(double x) const noexcept;
	
private:
	double m_minInterval{};
	double m_maxInterval{};
	static const Interval m_emptyInterval;
	static const Interval m_universalInterval;
};