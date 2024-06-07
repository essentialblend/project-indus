export module h_samplingutils;

import <cmath>;

import h_mathutilities;	

export
{
	Float sampleVisibleWavelengths(const Float u)
	{
		return 538 - 138.888889f * std::atanh(0.85691062f - 1.82750197f * u);
	}

	Float visibleWavelengthsPDF(const Float lambda)
	{
		if (lambda < 360 || lambda > 830)
		{
			return 0;
		}

		return 0.0039398042f / std::pow(std::cosh(0.0072f * (lambda - 538)), 2);
	}
};