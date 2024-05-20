export module ri_configurable;

export class RIConfigurable abstract
{
public:
	virtual void configureEntity() = 0;

	virtual ~RIConfigurable() noexcept = default;
};