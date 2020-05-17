#include "ecsComponent.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use our shared namespace mini
using namespace mini;

///////////////////////////////////////////////////////////////////////////
/// registerType
///////////////////////////////////////////////////////////////////////////

#include <iostream>

ComponentID ecsBaseComponent::registerType(
    const ComponentCreateFunction& createFn,
    const ComponentFreeFunction& freeFn, const size_t& size) {
    const auto componentID =
        static_cast<ComponentID>(m_componentRegistry.size());

    std::cout << size << std::endl;
    m_componentRegistry.emplace_back(createFn, freeFn, size);

    return componentID;
}