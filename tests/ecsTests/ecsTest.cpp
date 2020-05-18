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

int main() noexcept {
    FooComponent foo = FooComponent();
    BarComponent bar = BarComponent();
    ecsWorld world = ecsWorld();

    const auto entityHandle = world.makeEntity(nullptr, 0);
    world.makeComponent(entityHandle, &foo);
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
    return 0;
}