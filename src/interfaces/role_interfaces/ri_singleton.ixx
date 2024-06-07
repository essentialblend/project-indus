export module ri_singleton;

export template<typename EntityType>
class RISingleton
{
public:
	static EntityType& getSingletonInstance() noexcept
	{
		static EntityType staticInstance{};
		return staticInstance;
	}
	virtual ~RISingleton() noexcept = default;
};