#include "ecsComponent.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use our shared namespace mini
using namespace mini;

///////////////////////////////////////////////////////////////////////////
/// registerType
///////////////////////////////////////////////////////////////////////////

ComponentID ecsBaseComponent::registerType(
    const ComponentCreateFunction& createFn,
    const ComponentFreeFunction& freeFn, const size_t& size) {
    auto componentID = static_cast<ComponentID>(m_componentRegistry.size());
    m_componentRegistry.emplace_back(createFn, freeFn, size);

    return componentID;
}