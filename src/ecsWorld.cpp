#include "ecsWorld.hpp"
#include <random>
#include <sstream>

///////////////////////////////////////////////////////////////////////////
/// Use our shared namespace mini
using namespace mini;

///////////////////////////////////////////////////////////////////////////
/// makeEntity
///////////////////////////////////////////////////////////////////////////

EntityHandle ecsWorld::makeEntity(
    const ecsBaseComponent* const* const components,
    const size_t& numComponents) {
    const auto UUID = EntityHandle(generateUUID());

    auto newEntity = std::make_shared<ecsEntity>();
    m_entities.insert_or_assign(UUID, newEntity);

    for (size_t i = 0; i < numComponents; ++i) {
        makeComponent(UUID, components[i]);
    }

    return UUID;
}

///////////////////////////////////////////////////////////////////////////
/// makeComponent
///////////////////////////////////////////////////////////////////////////

ComponentHandle ecsWorld::makeComponent(
    const EntityHandle& entityHandle, const ecsBaseComponent* const component) {
    return makeComponent(entityHandle, component->m_runtimeID, component);
}

///////////////////////////////////////////////////////////////////////////

ComponentHandle ecsWorld::makeComponent(
    const EntityHandle& entityHandle, const ComponentID& componentID,
    const ecsBaseComponent* const component) {
    // Check if entity is valid
    if (const auto entity = getEntity(entityHandle)) {
        // Check if component ID is valid
        if (isComponentIDValid(componentID)) {
            // Prevent adding duplicate component types to the same entity
            for (const auto& [ID, fn, compHandle] : entity->m_components)
                if (ID == componentID) {
                    return compHandle;
                }

            // Create new instance of this component
            const auto UUID = ComponentHandle(generateUUID());
            const auto& createfn =
                std::get<0>(ecsBaseComponent::m_componentRegistry[componentID]);
            entity->m_components.emplace_back(
                componentID,
                createfn(
                    m_components[componentID], UUID, entityHandle, component),
                UUID);
            return UUID;
        }
    }

    return ComponentHandle();
}

///////////////////////////////////////////////////////////////////////////
/// removeEntity
///////////////////////////////////////////////////////////////////////////

bool ecsWorld::removeEntity(const EntityHandle& entityHandle) {
    // Delete this entity's components
    if (const auto entity = getEntity(entityHandle)) {
        for (auto& [id, createFn, componentHandle] : entity->m_components)
            deleteComponent(id, createFn);

        // Delete this entity
        m_entities.erase(entityHandle);
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////
/// removeComponent
///////////////////////////////////////////////////////////////////////////

bool ecsWorld::removeComponent(const ComponentHandle& componentHandle) {
    // Check if the component handle is valid
    if (const auto& component = getComponent(componentHandle))
        return removeEntityComponent(
            component->m_entityHandle, component->m_runtimeID);
    return false;
}

///////////////////////////////////////////////////////////////////////////
/// removeEntityComponent
///////////////////////////////////////////////////////////////////////////

bool ecsWorld::removeEntityComponent(
    const EntityHandle& entityHandle, const ComponentID& componentID) {
    // Check if entity handle is valid
    if (const auto entity = getEntity(entityHandle)) {
        auto& entityComponents = entity->m_components;
        const auto entityComponentCount = entityComponents.size();
        for (size_t i = 0ULL; i < entityComponentCount; ++i) {
            const auto& [compId, fn, compHandle] = entityComponents[i];
            if (componentID == compId) {
                deleteComponent(compId, fn);
                const auto srcIndex = entityComponents.size() - 1ULL;
                const auto destIndex = i;
                entityComponents[destIndex] = entityComponents[srcIndex];
                entityComponents.pop_back();
                return true;
            }
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////
/// getEntity
///////////////////////////////////////////////////////////////////////////

std::shared_ptr<ecsEntity> ecsWorld::getEntity(const EntityHandle& UUID) const {
    const auto pos = m_entities.find(UUID);
    if (pos != m_entities.end())
        return pos->second;
    return {};
}

///////////////////////////////////////////////////////////////////////////
/// getEntities
///////////////////////////////////////////////////////////////////////////

std::vector<std::shared_ptr<ecsEntity>>
ecsWorld::getEntities(const std::vector<EntityHandle>& uuids) const {
    std::vector<std::shared_ptr<ecsEntity>> entities;
    entities.reserve(uuids.size());
    for (const auto& uuid : uuids)
        if (const auto entity = getEntity(uuid))
            entities.push_back(entity);
    return entities;
}

///////////////////////////////////////////////////////////////////////////
/// getComponent
///////////////////////////////////////////////////////////////////////////

ecsBaseComponent* ecsWorld::getComponent(
    const EntityHandle& entityHandle, const ComponentID& componentID) const {
    if (const auto entity = getEntity(entityHandle))
        return getComponent(
            entity->m_components, m_components.at(componentID), componentID);
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////
/// getComponent
///////////////////////////////////////////////////////////////////////////

ecsBaseComponent*
ecsWorld::getComponent(const ComponentHandle& componentHandle) const {
    // Search all entities in the list supplied
    for (const auto& [entityHandle, entity] : m_entities) {
        // Check if this entity contains the component handle
        for (const auto& [compID, fn, compHandle] : entity->m_components)
            if (compHandle == componentHandle)
                return (ecsBaseComponent*)(&(m_components.at(compID)[fn]));
    }
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////
/// getComponent
///////////////////////////////////////////////////////////////////////////

ecsBaseComponent* ecsWorld::getComponent(
    const std::vector<std::tuple<ComponentID, int, ComponentHandle>>&
        entityComponents,
    const ComponentDataSpace& mem_array,
    const ComponentID& componentID) noexcept {
    for (const auto& entityComponent : entityComponents) {
        const auto& [compId, fn, compHandle] = entityComponent;
        if (componentID == compId)
            return (ecsBaseComponent*)(&mem_array[fn]);
    }
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////
/// operator=
///////////////////////////////////////////////////////////////////////////

ecsWorld& ecsWorld::operator=(ecsWorld&& other) noexcept {
    if (this != &other) {
        m_components = std::move(other.m_components);
        m_entities = std::move(other.m_entities);
    }
    return *this;
}

///////////////////////////////////////////////////////////////////////////
/// clear
///////////////////////////////////////////////////////////////////////////

void ecsWorld::clear() noexcept {
    // Remove all components
    for (auto& m_component : m_components) {
        const auto& [createFn, freeFn, typeSize] =
            ecsBaseComponent::m_componentRegistry[m_component.first];
        const auto componentContainerSize = m_component.second.size();
        for (size_t i = 0; i < componentContainerSize; i += typeSize)
            freeFn(reinterpret_cast<ecsBaseComponent*>(&m_component.second[i]));
        m_component.second.clear();
    }
    m_components.clear();

    // Remove all entities
    m_entities.clear();
}

///////////////////////////////////////////////////////////////////////////
/// generateUUID
///////////////////////////////////////////////////////////////////////////

ecsHandle ecsWorld::generateUUID() {
    std::stringstream ss;
    for (auto i = 0; i < 16; i++) {
        std::random_device rd;
        std::mt19937 gen(rd());
        const std::uniform_int_distribution<int> dis(0, 255);
        const auto rc = dis(gen);
        std::stringstream hexstream;
        hexstream << std::hex << rc;
        const auto hex = hexstream.str();
        ss << (hex.length() < 2 ? '0' + hex : hex);
    }
    const auto& string = ss.str();
    ecsHandle handle;
    std::copy(&string[0], &string[32], &handle.m_uuid[0]);
    return handle;
}

///////////////////////////////////////////////////////////////////////////
/// isComponentIDValid
///////////////////////////////////////////////////////////////////////////

bool ecsWorld::isComponentIDValid(const ComponentID& componentID) noexcept {
    return (
        static_cast<size_t>(componentID) <
        ecsBaseComponent::m_componentRegistry.size());
}

///////////////////////////////////////////////////////////////////////////
/// deleteComponent
///////////////////////////////////////////////////////////////////////////

void ecsWorld::deleteComponent(
    const ComponentID& componentID, const ComponentID& index) {
    if (isComponentIDValid(componentID)) {
        auto& mem_array = m_components[componentID];
        const auto& [createFn, freeFn, typeSize] =
            ecsBaseComponent::m_componentRegistry[componentID];
        const auto srcIndex = mem_array.size() - typeSize;

        const auto* srcComponent =
            reinterpret_cast<ecsBaseComponent*>(&mem_array[srcIndex]);
        auto* destComponent =
            reinterpret_cast<ecsBaseComponent*>(&mem_array[index]);
        freeFn(destComponent);

        if (static_cast<size_t>(index) == srcIndex) {
            mem_array.resize(srcIndex);
            return;
        }
        std::memcpy(destComponent, srcComponent, typeSize);

        // Update references
        for (auto& component :
             getEntity(srcComponent->m_entityHandle)->m_components) {
            auto& [compID, fn, compHandle] = component;
            if (componentID == compID &&
                static_cast<ComponentID>(srcIndex) == fn) {
                fn = index;
                break;
            }
        }
        mem_array.resize(srcIndex);
    }
}

///////////////////////////////////////////////////////////////////////////
/// updateSystems
///////////////////////////////////////////////////////////////////////////

void ecsWorld::updateSystems(ecsSystemList& systems, const double& deltaTime) {
    for (auto& system : systems)
        updateSystem(system.get(), deltaTime);
}

///////////////////////////////////////////////////////////////////////////
/// updateSystem
///////////////////////////////////////////////////////////////////////////

void ecsWorld::updateSystem(ecsSystem* system, const double& deltaTime) {
    if (auto components = getRelevantComponents(system->getComponentTypes());
        !components.empty())
        system->updateComponents(deltaTime, components);
}

///////////////////////////////////////////////////////////////////////////

void ecsWorld::updateSystem(
    const std::shared_ptr<ecsSystem>& system, const double& deltaTime) {
    updateSystem(system.get(), deltaTime);
}

///////////////////////////////////////////////////////////////////////////

void ecsWorld::updateSystem(
    const double& deltaTime,
    const std::vector<std::pair<ComponentID, ecsSystem::RequirementsFlag>>&
        componentTypes,
    const std::function<void(
        const double&, const std::vector<std::vector<ecsBaseComponent*>>&)>&
        func) {
    if (auto components = getRelevantComponents(componentTypes);
        !components.empty())
        func(deltaTime, components);
}

///////////////////////////////////////////////////////////////////////////
/// getRelevantComponents
///////////////////////////////////////////////////////////////////////////

std::vector<std::vector<ecsBaseComponent*>> ecsWorld::getRelevantComponents(
    const std::vector<std::pair<ComponentID, ecsSystem::RequirementsFlag>>&
        componentTypes) {
    std::vector<std::vector<ecsBaseComponent*>> components;
    if (!componentTypes.empty()) {
        const auto componentTypesCount = componentTypes.size();
        if (componentTypesCount == 1U) {
            // Super simple procedure for system with 1 component type
            const auto& [componentID, componentFlag] = componentTypes[0];
            const auto& [createFn, freeFn, typeSize] =
                ecsBaseComponent::m_componentRegistry[componentID];
            const auto& mem_array = m_components[componentID];
            const auto mem_arraySize = mem_array.size();
            components.resize(mem_arraySize / typeSize);
            for (size_t j = 0, k = 0; j < mem_arraySize; j += typeSize, ++k)
                components[k].push_back((ecsBaseComponent*)(&mem_array[j]));
        } else {
            // More complex procedure for system with > 1 component type
            std::vector<ecsBaseComponent*> componentParam(componentTypesCount);
            std::vector<ComponentDataSpace*> componentArrays(
                componentTypesCount);
            for (size_t i = 0; i < componentTypesCount; ++i)
                componentArrays[i] =
                    &m_components[std::get<0>(componentTypes[i])];

            const auto minSizeIndex = findLeastCommonComponent(componentTypes);
            const auto minComponentID =
                std::get<0>(componentTypes[minSizeIndex]);
            const auto& [createFn, freeFn, typeSize] =
                ecsBaseComponent::m_componentRegistry[minComponentID];
            const auto& mem_array = *componentArrays[minSizeIndex];
            const auto mem_arraySize = mem_array.size();
            // reserve, not resize, as the component at [i] may be invalid
            components.reserve(mem_arraySize / typeSize);
            for (size_t i = 0; i < mem_arraySize; i += typeSize) {
                componentParam[minSizeIndex] =
                    (ecsBaseComponent*)(&mem_array[i]);
                if (const auto entity = getEntity(
                        componentParam[minSizeIndex]->m_entityHandle)) {
                    const auto& entityComponents = entity->m_components;

                    bool isValid = true;
                    for (size_t j = 0; j < componentTypesCount; ++j) {
                        const auto& [componentID, componentFlag] =
                            componentTypes[j];
                        if (j == minSizeIndex)
                            continue;
                        componentParam[j] = getComponent(
                            entityComponents, *componentArrays[j], componentID);
                        if ((componentParam[j] == nullptr) &&
                            (static_cast<unsigned int>(componentFlag) &
                             static_cast<unsigned int>(
                                 ecsSystem::RequirementsFlag::FLAG_OPTIONAL)) ==
                                0) {
                            isValid = false;
                            break;
                        }
                    }
                    if (isValid)
                        components.push_back(componentParam);
                }
            }
        }
    }
    return components;
}

///////////////////////////////////////////////////////////////////////////
/// findLeastCommonComponent
///////////////////////////////////////////////////////////////////////////

size_t ecsWorld::findLeastCommonComponent(
    const std::vector<std::pair<ComponentID, ecsSystem::RequirementsFlag>>&
        componentTypes) {
    auto minSize = std::numeric_limits<size_t>::max();
    auto minIndex = std::numeric_limits<size_t>::max();
    const auto componentTypesCount = componentTypes.size();
    for (size_t i = 0; i < componentTypesCount; ++i) {
        const auto& [componentID, componentFlag] = componentTypes[i];
        if ((static_cast<unsigned int>(componentFlag) &
             static_cast<unsigned int>(
                 ecsSystem::RequirementsFlag::FLAG_OPTIONAL)) != 0)
            continue;

        const auto& [createFn, freeFn, typeSize] =
            ecsBaseComponent::m_componentRegistry[componentID];
        const auto size = m_components[componentID].size() / typeSize;
        if (size <= minSize) {
            minSize = size;
            minIndex = i;
        }
    }
    return minIndex;
}