#pragma once

class Interval
{
public:
	Interval() : minRange(-Uinf), maxRange(+Uinf) {}
	Interval(double m, double ma) : minRange(m), maxRange(ma) {}

	bool isWithinInterval(double inputV) const
	{
		return (minRange <= inputV  && inputV <= maxRange);
	}

	bool isStrictlyWithinInterval(double inputV) const
	{
		return (minRange < inputV && inputV < maxRange);
	}

	double getIntervalMinRange() const
 	{
		return minRange;
	}

	double getIntervalMaxRange() const
	{
		return maxRange;
	}

private:
	double minRange;
	double maxRange;
};