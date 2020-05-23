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
    const size_t numComponents) {
    auto UUID = EntityHandle(generateUUID());

    m_entities.insert_or_assign(
        UUID, std::make_shared<ecsEntity>(ecsEntity{ UUID, {} }));

    for (size_t i = 0; i < numComponents; ++i)
        [[maybe_unused]] const auto componentHandle =
            makeComponent(UUID, components[i]);

    return UUID;
}

///////////////////////////////////////////////////////////////////////////
/// makeComponent
///////////////////////////////////////////////////////////////////////////

ComponentHandle ecsWorld::makeComponent(
    const EntityHandle& entityHandle, const ecsBaseComponent* const component) {
    return makeComponentInternal(
        entityHandle, component->m_runtimeID, component);
}

///////////////////////////////////////////////////////////////////////////

ComponentHandle mini::ecsWorld::makeComponent(
    ecsEntity& entity, const ecsBaseComponent* const component) {
    return makeComponentInternal(entity, component->m_runtimeID, component);
}

///////////////////////////////////////////////////////////////////////////
/// removeEntity
///////////////////////////////////////////////////////////////////////////

bool ecsWorld::removeEntity(const EntityHandle& entityHandle) {
    const auto entity = getEntity(entityHandle);
    if (entity == nullptr)
        return false;

    return removeEntity(*entity);
}

///////////////////////////////////////////////////////////////////////////

bool mini::ecsWorld::removeEntity(ecsEntity& entity) {
    // Delete this entity's components
    for (auto& [id, createFn, componentHandle] : entity.m_components)
        removeComponentInternal(id, createFn);

    // Delete this entity
    m_entities.erase(entity.m_handle);
    return true;
}

///////////////////////////////////////////////////////////////////////////
/// removeComponent
///////////////////////////////////////////////////////////////////////////

bool ecsWorld::removeComponent(const ComponentHandle& componentHandle) {
    // Check if the component handle is valid
    const auto& component = getComponent(componentHandle);
    if (component == nullptr)
        return false;

    return removeComponentInternal(
        component->m_entityHandle, component->m_runtimeID);
}

///////////////////////////////////////////////////////////////////////////
/// getEntity
///////////////////////////////////////////////////////////////////////////

std::shared_ptr<ecsEntity> ecsWorld::getEntity(const EntityHandle& UUID) const {
    if (!UUID.isValid())
        return {};

    const auto pos = m_entities.find(UUID);
    if (pos == m_entities.end())
        return {};

    return pos->second;
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
            entities.emplace_back(entity);
    return entities;
}

///////////////////////////////////////////////////////////////////////////
/// getComponent
///////////////////////////////////////////////////////////////////////////

ecsBaseComponent*
ecsWorld::getComponent(const ComponentHandle& componentHandle) {
    if (!componentHandle.isValid())
        return nullptr;

    // Search all entities in the list supplied
    for (const auto& [entityHandle, entity] : m_entities) {
        // Check if this entity contains the component handle
        for (const auto& [compID, fn, compHandle] : entity->m_components)
            if (compHandle == componentHandle)
                return reinterpret_cast<ecsBaseComponent*>(
                    &(m_components[compID].at(fn)));
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

void ecsWorld::clear() {
    // Remove all components
    for (auto& m_component : m_components) {
        const auto& [createFn, freeFn, typeSize] =
            ecsBaseComponent::m_componentRegistry[m_component.first];
        const auto containerSize = m_component.second.size();
        for (size_t i = 0; i < containerSize; i += typeSize)
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
    std::stringstream stringStream;
    for (auto i = 0; i < 16; i++) {
        std::random_device random;
        std::mt19937 generator(random());
        std::uniform_int_distribution<int> distribution(0, 255);
        const auto randomChar = distribution(generator);
        std::stringstream hexstream;
        hexstream << std::hex << randomChar;
        const auto hex = hexstream.str();
        stringStream << (hex.length() < 2 ? '0' + hex : hex);
    }
    const auto& string = stringStream.str();
    ecsHandle handle;
    std::copy(&string[0], &string[32], &handle.m_uuid[0]);
    return handle;
}

///////////////////////////////////////////////////////////////////////////
/// isComponentIDValid
///////////////////////////////////////////////////////////////////////////

bool ecsWorld::isComponentIDValid(const ComponentID& componentID) noexcept {
    return static_cast<size_t>(componentID) <
           ecsBaseComponent::m_componentRegistry.size();
}

///////////////////////////////////////////////////////////////////////////
/// makeComponentInternal
///////////////////////////////////////////////////////////////////////////

ComponentHandle ecsWorld::makeComponentInternal(
    const EntityHandle& entityHandle, const ComponentID& componentID,
    const ecsBaseComponent* const component) {
    const auto entity = getEntity(entityHandle);
    if (entity == nullptr)
        return ComponentHandle();

    return makeComponentInternal(*entity, componentID, component);
}

///////////////////////////////////////////////////////////////////////////

ComponentHandle mini::ecsWorld::makeComponentInternal(
    ecsEntity& entity, const ComponentID& componentID,
    const ecsBaseComponent* const component) {
    // Check if component ID is valid
    if (!isComponentIDValid(componentID))
        return ComponentHandle();

    // Prevent adding duplicate component types to the same entity
    for (auto& [ID, fn, compHandle] : entity.m_components)
        if (ID == componentID)
            return compHandle;

    // Create new instance of this component
    auto UUID = ComponentHandle(generateUUID());
    const auto& createfn =
        std::get<0>(ecsBaseComponent::m_componentRegistry[componentID]);
    entity.m_components.emplace_back(
        componentID,
        createfn(m_components[componentID], UUID, entity.m_handle, component),
        UUID);
    return UUID;
}

///////////////////////////////////////////////////////////////////////////
/// removeComponentInternal
///////////////////////////////////////////////////////////////////////////

bool ecsWorld::removeComponentInternal(
    const EntityHandle& entityHandle, const ComponentID& componentID) {
    const auto entity = getEntity(entityHandle);
    if (entity == nullptr)
        return false;

    return removeComponentInternal(*entity, componentID);
}

///////////////////////////////////////////////////////////////////////////

bool mini::ecsWorld::removeComponentInternal(
    ecsEntity& entity, const ComponentID& componentID) {
    auto& entityComponents = entity.m_components;
    const auto entityComponentCount = entityComponents.size();
    for (size_t i = 0ULL; i < entityComponentCount; ++i) {
        const auto& [compId, fn, compHandle] = entityComponents[i];
        if (compId == componentID) {
            removeComponentInternal(compId, fn);
            const auto srcIndex = entityComponents.size() - 1ULL;
            const auto destIndex = i;
            entityComponents[destIndex] = entityComponents[srcIndex];
            entityComponents.pop_back();
            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////

void ecsWorld::removeComponentInternal(
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
        std::memcpy(
            static_cast<void*>(destComponent),
            static_cast<const void*>(srcComponent), typeSize);

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
/// getComponentInternal
///////////////////////////////////////////////////////////////////////////

ecsBaseComponent* ecsWorld::getComponentInternal(
    const EntityHandle& entityHandle, const ComponentID& componentID) {
    const auto entity = getEntity(entityHandle);
    if (entity == nullptr)
        return nullptr;

    return getComponentInternal(*entity, componentID);
}

///////////////////////////////////////////////////////////////////////////

ecsBaseComponent* mini::ecsWorld::getComponentInternal(
    ecsEntity& entity, const ComponentID& componentID) {
    return getComponentInternal(
        entity.m_components, m_components[componentID], componentID);
}

///////////////////////////////////////////////////////////////////////////

ecsBaseComponent* ecsWorld::getComponentInternal(
    const std::vector<std::tuple<ComponentID, int, ComponentHandle>>&
        entityComponents,
    ComponentDataSpace& mem_array, const ComponentID& componentID) noexcept {
    for (const auto& entityComponent : entityComponents) {
        const auto& [compId, fn, compHandle] = entityComponent;
        if (componentID == compId)
            return reinterpret_cast<ecsBaseComponent*>(&mem_array[fn]);
    }
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////
/// updateSystems
///////////////////////////////////////////////////////////////////////////

void ecsWorld::updateSystems(ecsSystemList& systems, const double& deltaTime) {
    for (auto& system : systems)
        updateSystem(*system, deltaTime);
}

///////////////////////////////////////////////////////////////////////////
/// updateSystem
///////////////////////////////////////////////////////////////////////////

void ecsWorld::updateSystem(ecsSystem& system, const double& deltaTime) {
    if (const auto components =
            getRelevantComponents(system.getComponentTypes());
        !components.empty())
        system.updateComponents(deltaTime, components);
}

///////////////////////////////////////////////////////////////////////////

void ecsWorld::updateSystem(
    const double& deltaTime,
    const std::vector<std::pair<ComponentID, ecsSystem::RequirementsFlag>>&
        componentTypes,
    const std::function<void(
        const double&, const std::vector<std::vector<ecsBaseComponent*>>&)>&
        func) {
    if (const auto components = getRelevantComponents(componentTypes);
        !components.empty())
        func(deltaTime, components);
}

///////////////////////////////////////////////////////////////////////////
/// getRelevantComponents
///////////////////////////////////////////////////////////////////////////

std::vector<std::vector<ecsBaseComponent*>> ecsWorld::getRelevantComponents(
    const std::vector<std::pair<ComponentID, ecsSystem::RequirementsFlag>>&
        componentTypes) {
    if (componentTypes.empty())
        return {};

    std::vector<std::vector<ecsBaseComponent*>> components;
    const auto componentTypesCount = componentTypes.size();
    // Super simple procedure for system with 1 component type
    if (componentTypesCount == 1U) {
        const auto& componentID = componentTypes.front().first;
        const auto& typeSize =
            std::get<2>(ecsBaseComponent::m_componentRegistry[componentID]);
        auto& mem_array = m_components[componentID];
        const auto mem_arraySize = mem_array.size();
        components.reserve(mem_arraySize / typeSize);
        for (size_t j = 0; j < mem_arraySize; j += typeSize)
            components.emplace_back(std::vector<ecsBaseComponent*>{
                reinterpret_cast<ecsBaseComponent*>(&mem_array[j]) });
    }
    // More complex procedure for system with > 1 component type
    else {
        // Accumulate component class data pointers
        std::vector<ComponentDataSpace*> componentArrays;
        componentArrays.reserve(componentTypesCount);
        std::transform(
            componentTypes.cbegin(), componentTypes.cend(),
            std::back_inserter(componentArrays),
            [&](const auto& type) { return &m_components[std::get<0>(type)]; });

        const auto minSizeIndex = findLeastCommonComponent(componentTypes);
        const auto minComponentID = std::get<0>(componentTypes[minSizeIndex]);
        const auto& typeSize =
            std::get<2>(ecsBaseComponent::m_componentRegistry[minComponentID]);
        auto& mem_array = *componentArrays[minSizeIndex];
        const auto mem_arraySize = mem_array.size();
        components.reserve(mem_arraySize / typeSize);

        // Find all relevant components, store in componentParam
        std::vector<ecsBaseComponent*> componentParam(componentTypesCount);
        for (size_t i = 0; i < mem_arraySize; i += typeSize) {
            componentParam[minSizeIndex] =
                reinterpret_cast<ecsBaseComponent*>(&mem_array[i]);
            const auto entity =
                getEntity(componentParam[minSizeIndex]->m_entityHandle);
            if (entity == nullptr)
                continue;

            const auto& entityComponents = entity->m_components;
            bool isValid = true;
            for (size_t j = 0; j < componentTypesCount; ++j) {
                if (j == minSizeIndex)
                    continue;

                const auto& [componentID, componentFlag] = componentTypes[j];
                componentParam[j] = getComponentInternal(
                    entityComponents, *componentArrays[j], componentID);
                if (componentParam[j] == nullptr &&
                    componentFlag == ecsSystem::RequirementsFlag::REQUIRED) {
                    isValid = false;
                    break;
                }
            }
            if (isValid)
                components.emplace_back(componentParam);
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
    size_t minSize = std::numeric_limits<size_t>::max();
    size_t minIndex = std::numeric_limits<size_t>::max();
    size_t index(0ULL);

    for (const auto& [componentID, componentFlag] : componentTypes) {
        if (componentFlag == ecsSystem::RequirementsFlag::OPTIONAL)
            continue;

        const auto& typeSize =
            std::get<2>(ecsBaseComponent::m_componentRegistry[componentID]);
        const auto size = m_components[componentID].size() / typeSize;
        if (size <= minSize) {
            minSize = size;
            minIndex = index;
        }
        ++index;
    }
    return minIndex;
}