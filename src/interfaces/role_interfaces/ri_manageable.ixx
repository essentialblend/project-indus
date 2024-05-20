export module ri_manageable;

export class RIManageable abstract
{
public:
	virtual void manageEntityResources() = 0;

	virtual ~RIManageable() noexcept = default;
};