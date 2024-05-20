export module ri_initializable;

export class RIInitializable abstract
{
public:
	virtual void initializeEntity() = 0;

	virtual ~RIInitializable() noexcept = default;
};