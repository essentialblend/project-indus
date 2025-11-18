export module indus.ui.header_system;

import std;

import indus.ui.hud_system;
import indus.ui.hud_component;
import indus.ui.enginelogo_component;

export class HeaderSystem final : public HUDSystem
{
public:
  explicit HeaderSystem(std::vector<std::unique_ptr<HUDComponent>> components, HUDSystemType systemType = HUDSystemType::Header);
};

HeaderSystem::HeaderSystem(std::vector<std::unique_ptr<HUDComponent>> components, HUDSystemType systemType) : HUDSystem{ std::move(components), systemType } {}
