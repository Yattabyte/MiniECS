#include "ecsSystem.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use our shared namespace mini
using namespace mini;

///////////////////////////////////////////////////////////////////////////
/// isValid
///////////////////////////////////////////////////////////////////////////

bool ecsSystem::isValid() const noexcept {
    for (const auto& [componentID, componentFlag] : m_componentTypes) {
        if (componentFlag == RequirementsFlag::REQUIRED) {
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////
/// addComponentType
///////////////////////////////////////////////////////////////////////////

void ecsSystem::addComponentType(const ComponentID componentType, const RequirementsFlag componentFlag) {
    m_componentTypes.emplace_back(componentType, componentFlag);
}

///////////////////////////////////////////////////////////////////////////
/// addSystem
///////////////////////////////////////////////////////////////////////////

bool ecsSystemList::addSystem(const std::shared_ptr<ecsSystem>& system) {
    if (system->isValid()) {
        m_systems.emplace_back(system);
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////
/// removeSystem
///////////////////////////////////////////////////////////////////////////

bool ecsSystemList::removeSystem(const ecsSystem& system) {
    const auto systemCount = m_systems.size();
    for (size_t i = 0; i < systemCount; ++i) {
        if (&system == m_systems[i].get()) {
            m_systems.erase(m_systems.begin() + i);
            return true;
        }
    }
    return false;
}