export module ri_initializable;

import <vector>;
import <any>;

export class RIInitializable abstract
{
public:
	virtual void initializeEntity([[maybe_unused]] const std::vector<std::any>& args = {}) = 0;

	virtual ~RIInitializable() noexcept = default;
};