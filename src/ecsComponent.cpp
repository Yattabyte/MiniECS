#include "ecsComponent.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use our shared namespace mini
using namespace mini;

///////////////////////////////////////////////////////////////////////////
/// registerType
///////////////////////////////////////////////////////////////////////////

ComponentID ecsBaseComponent::registerType(
    const ComponentCreateFunction& createFn,
    const ComponentFreeFunction& freeFn, const ComponentNewFunction& newFn,
    const size_t& size) {
    const auto componentID =
        static_cast<ComponentID>(m_componentRegistry.size());

    m_componentRegistry.emplace_back(createFn, freeFn, newFn, size);

    return componentID;
}