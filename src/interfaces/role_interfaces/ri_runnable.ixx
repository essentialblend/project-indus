export module ri_runnable;

export class RIStartable abstract
{
public:
	virtual void startEntity() = 0;

	virtual ~RIStartable() noexcept = default;
};

export class RIStoppable abstract
{
public:
	virtual void stopEntity() = 0;

	virtual ~RIStoppable() noexcept = default;
};

export class RIRunnable abstract : public RIStartable, public RIStoppable
{
public:
	virtual void startEntity() = 0;
	virtual void stopEntity() = 0;

	virtual ~RIRunnable() noexcept = default;
};