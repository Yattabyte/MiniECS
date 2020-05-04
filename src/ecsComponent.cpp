#include "ecsComponent.hpp"

ecsBaseComponent::ecsBaseComponent(
    const ComponentID& ID, const size_t& size) noexcept
    : m_runtimeID(ID), m_size(size) {}

ComponentID ecsBaseComponent::registerType(
    const ComponentCreateFunction& createFn,
    const ComponentFreeFunction& freeFn, const ComponentNewFunction& newFn,
    const size_t& size) {
    const auto componentID =
        static_cast<ComponentID>(m_componentRegistry.size());

    m_componentRegistry.emplace_back(createFn, freeFn, newFn, size);

    return componentID;
}