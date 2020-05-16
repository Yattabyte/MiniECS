#include "ecsWorld.hpp"
#include <assert.h>
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
    FooComponent f = FooComponent();
    BarComponent b = BarComponent();
    ecsWorld world = ecsWorld();

    const auto entityHandle = world.makeEntity(nullptr, 0);
    world.makeComponent(entityHandle, &f);
    // world.makeComponent(entityHandle, &b);

    [[maybe_unused]] const std::vector<
        std::pair<ComponentID, ecsSystem::RequirementsFlag>>
        ids = { { FooComponent::Runtime_ID,
                  ecsSystem::RequirementsFlag::FLAG_REQUIRED },
                { BarComponent::Runtime_ID,
                  ecsSystem::RequirementsFlag::FLAG_OPTIONAL } };

    /*[[maybe_unused]] const auto qwe =
        world.getComponents<FooComponent*, BarComponent*>(
            { { FooComponent::Runtime_ID,
                ecsSystem::RequirementsFlag::FLAG_REQUIRED },
              { BarComponent::Runtime_ID,
                ecsSystem::RequirementsFlag::FLAG_OPTIONAL } });

    assert(qwe.size() == 1);
    assert(std::get<0>(qwe[0]) != nullptr);
    assert(std::get<1>(qwe[0]) == nullptr);*/
    return 0;
}