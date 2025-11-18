export module indus.ui.renderconfig_system;

import std;

import indus.ui.hud_system;
import indus.ui.hud_component;

export class RenderConfigSystem final : public HUDSystem
{
public:
  explicit RenderConfigSystem(std::vector<std::unique_ptr<HUDComponent>> components, HUDSystemType systemType = HUDSystemType::RenderConfig);
};

RenderConfigSystem::RenderConfigSystem(std::vector<std::unique_ptr<HUDComponent>> components, HUDSystemType systemType) : HUDSystem{ std::move(components), systemType } {}


