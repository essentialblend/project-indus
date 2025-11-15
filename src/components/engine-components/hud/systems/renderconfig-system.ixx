export module renderconfigsystem;

import std;
import hudsystem;
import hudcomponent;

export class RenderConfigSystem final : public HUDSystem
{
public:
  explicit RenderConfigSystem(std::vector<std::unique_ptr<HUDComponent>> components, HUDSystemType systemType = HUDSystemType::RenderConfig);
};

RenderConfigSystem::RenderConfigSystem(std::vector<std::unique_ptr<HUDComponent>> components, HUDSystemType systemType) : HUDSystem{ std::move(components), systemType } {}


