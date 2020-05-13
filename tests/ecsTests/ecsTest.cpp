#include "ecsWorld.hpp"
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
    FooComponent f;
    BarComponent b;

    ecsWorld world;
    auto entityHandle = world.makeEntity(nullptr, 0);
    world.makeComponent(entityHandle, &f);
    // world.makeComponent(entityHandle, &b);

    [[maybe_unused]] const auto qwe =
        world.getComponents<FooComponent*, BarComponent*>(
            { { FooComponent::Runtime_ID,
                ecsSystem::RequirementsFlag::FLAG_REQUIRED },
              { BarComponent::Runtime_ID,
                ecsSystem::RequirementsFlag::FLAG_OPTIONAL } });

    [[maybe_unused]] bool asd = false;
}