export module eu_objectfactory;

import i_engineutility;
import ri_singleton;

export class EUObjectFactory final : public IEngineUtility, public RISingleton<EUObjectFactory>
{
public:
private:

	friend class RISingleton<EUObjectFactory>;
};