#include "ecsWorld.hpp"
#include <cassert>
#include <iostream>
#include <string>

using namespace mini;

struct FooComponent : ecsComponent<FooComponent> {
    bool qwe = true;
};

struct BarComponent : ecsComponent<BarComponent> {
    bool asd = false;
};

class FooSystem : public ecsSystem {
    public:
    FooSystem() noexcept {
        addComponentType(FooComponent::Runtime_ID, RequirementsFlag::REQUIRED);
    }

    void updateComponents(
        const double& /*deltaTime*/,
        const std::vector<std::vector<ecsBaseComponent*>>& /*components*/)
        override {
        [[maybe_unused]] bool qwe = true;
    }
};

int main() noexcept {
    ecsWorld world = ecsWorld();

    [[maybe_unused]] const auto entityHandle = world.makeEntity();
    [[maybe_unused]] const auto componentHandle =
        world.makeComponent<FooComponent>(entityHandle);
    // world.makeComponent(entityHandle, &bar);

    [[maybe_unused]] const std::vector<
        std::pair<ComponentID, ecsSystem::RequirementsFlag>>
        ids = {
            { FooComponent::Runtime_ID, ecsSystem::RequirementsFlag::REQUIRED },
            { BarComponent::Runtime_ID, ecsSystem::RequirementsFlag::OPTIONAL }
        };
    [[maybe_unused]] const auto qwe =
        world.getComponents<FooComponent*, BarComponent*>(ids);

    assert(qwe.size() == 1);
    assert(std::get<0>(qwe[0]) != nullptr);
    assert(std::get<1>(qwe[0]) == nullptr);

    FooSystem system;
    world.updateSystem(system, 0.01);
    return 0;
}