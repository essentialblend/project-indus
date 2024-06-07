export module es_spectrum;

import <string>;
import <memory>;
import <optional>;

export class ECSampledWavelengths;
export class ECSampledSpectrum;

import i_enginesystem;
import h_mathutilities;

export class ESSpectrum abstract : public IEngineSystem
{
public:
	explicit ESSpectrum() noexcept = default;

	virtual Float operator()(const Float) const = 0;
	virtual ECSampledSpectrum sampleFromSpectrum(const ECSampledWavelengths&) const = 0;
	virtual Float getMaxComponentValue() const = 0;
	virtual ~ESSpectrum() noexcept = default;
};
