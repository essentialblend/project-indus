export module headersystem;

import std;
import hudcomponent;
import enginelogocomponent;
import hudsystem;

export class HeaderSystem final : public HUDSystem
{
public:
  explicit HeaderSystem(std::vector<std::unique_ptr<HUDComponent>> components, HUDSystemType systemType = HUDSystemType::Header);
};

HeaderSystem::HeaderSystem(std::vector<std::unique_ptr<HUDComponent>> components, HUDSystemType systemType) : HUDSystem{ std::move(components), systemType } {}
